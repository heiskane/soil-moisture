from listener.database import engine
from listener.models import Base


def main() -> None:
    Base.metadata.create_all(bind=engine, checkfirst=True)


if __name__ == "__main__":
    main()
