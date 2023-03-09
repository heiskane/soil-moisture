from database import engine

from listener.models import Base

Base.metadata.create_all(bind=engine, checkfirst=True)
