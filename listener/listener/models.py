from database import Base
from sqlalchemy import Column, DateTime, Integer, String


class SoilMoisture(Base):
    __tablename__ = "soil_moisture"

    id = Column(Integer, primary_key=True)
    time = Column(DateTime(timezone=True), index=True)
    sensor = Column(String)
    value = Column(Integer)
