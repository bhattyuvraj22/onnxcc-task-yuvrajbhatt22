"""
Generates a tiny fixture ONNX model and a matching input tensor, used by
onnxcc's C++ tests (see tests/fixtures/ and ONNXCC_FIXTURE_DIR in
tests/CMakeLists.txt).

Model: a 4 -> 8 -> 2 MLP with ReLU after each layer, built directly with
onnx.helper (NOT exported from PyTorch). PyTorch's nn.Linear exporter fuses
MatMul+Add into a single Gemm node, which Phase 1 of onnxcc does not
implement -- building the graph by hand avoids that entirely.

Graph:
    X (1,4) --MatMul(W1)--> --Add(B1)--> --Relu--> H (1,8)
             --MatMul(W2)--> --Add(B2)--> --Relu--> Y (1,2)

Node count: 6        (2x MatMul, 2x Add, 2x Relu)
Initializer count: 4 (W1, B1, W2, B2)
"""

from pathlib import Path

import numpy as np
import onnx
from onnx import checker, helper, TensorProto

SEED = 42
OPSET = 13

FIXTURE_DIR = Path(__file__).resolve().parent.parent / "tests" / "fixtures"
MODEL_PATH = FIXTURE_DIR / "mlp.onnx"
INPUT_PATH = FIXTURE_DIR / "input.bin"


def make_initializer(name: str, array: np.ndarray) -> onnx.TensorProto:
    return helper.make_tensor(
        name=name,
        data_type=TensorProto.FLOAT,
        dims=array.shape,
        vals=array.flatten().tolist(),
    )


def build_model() -> onnx.ModelProto:
    rng = np.random.default_rng(SEED)
    w1 = rng.standard_normal((4, 8)).astype(np.float32)
    b1 = rng.standard_normal((8,)).astype(np.float32)
    w2 = rng.standard_normal((8, 2)).astype(np.float32)
    b2 = rng.standard_normal((2,)).astype(np.float32)

    x_info = helper.make_tensor_value_info("X", TensorProto.FLOAT, [1, 4])
    y_info = helper.make_tensor_value_info("Y", TensorProto.FLOAT, [1, 2])

    nodes = [
        helper.make_node("MatMul", ["X", "W1"], ["mm1"], name="matmul1"),
        helper.make_node("Add", ["mm1", "B1"], ["add1"], name="add1"),
        helper.make_node("Relu", ["add1"], ["H"], name="relu1"),
        helper.make_node("MatMul", ["H", "W2"], ["mm2"], name="matmul2"),
        helper.make_node("Add", ["mm2", "B2"], ["add2"], name="add2"),
        helper.make_node("Relu", ["add2"], ["Y"], name="relu2"),
    ]

    graph = helper.make_graph(
        nodes=nodes,
        name="mlp_4_8_2",
        inputs=[x_info],
        outputs=[y_info],
        initializer=[
            make_initializer("W1", w1),
            make_initializer("B1", b1),
            make_initializer("W2", w2),
            make_initializer("B2", b2),
        ],
    )

    return helper.make_model(
        graph,
        opset_imports=[helper.make_opsetid("", OPSET)],
        producer_name="onnxcc-test-fixture-generator",
    )


def make_input() -> np.ndarray:
    rng = np.random.default_rng(SEED + 1)
    return rng.standard_normal((1, 4)).astype(np.float32)


def main() -> None:
    FIXTURE_DIR.mkdir(parents=True, exist_ok=True)

    model = build_model()
    checker.check_model(model)
    onnx.save(model, MODEL_PATH)

    x = make_input()
    INPUT_PATH.write_bytes(x.tobytes())

    print(f"wrote {MODEL_PATH}")
    print(f"  nodes: {len(model.graph.node)}, initializers: {len(model.graph.initializer)}")
    print(f"wrote {INPUT_PATH} ({x.nbytes} bytes, shape {x.shape})")


if __name__ == "__main__":
    main()