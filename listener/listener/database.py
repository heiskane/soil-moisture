from sqlalchemy import create_engine
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker

from listener.config import settings

engine = create_engine(settings.SQLALCHEMY_DATABASE_URI, echo=settings.DEBUG)

SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)

Base = declarative_base()
