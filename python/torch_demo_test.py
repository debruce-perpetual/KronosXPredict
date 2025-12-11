import kronospredict as kp


def main() -> None:
    arr = kp.torch_demo()
    print("torch_demo result shape:", arr.shape)
    print(arr)

    assert arr.shape == (2, 3), f"Expected shape (2, 3), got {arr.shape}"
    # dtype may be float32 or float64 depending on build; just ensure it's floating.
    assert str(arr.dtype).startswith("float"), f"Expected floating dtype, got {arr.dtype}"


if __name__ == "__main__":
    main()
