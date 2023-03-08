from pydantic import BaseModel


class SoilData(BaseModel):
    sensor: str
    value: int


def main() -> None:
    print("potato")


if __name__ == "__main__":
    main()
