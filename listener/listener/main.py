import json
import logging
import time
from datetime import datetime, timezone
from json import JSONDecodeError

import paho.mqtt.client as mqtt
from pydantic import BaseModel, ValidationError

from listener.config import settings
from listener.database import SessionLocal
from listener.models import SoilMoisture

logging.basicConfig(format="%(levelname)s: %(asctime)s %(message)s")
logger = logging.getLogger()
logger.setLevel(settings.LOG_LEVEL)


class SoilMoistureCreate(BaseModel):
    sensor: str
    value: int


class SoilMoistureDB(SoilMoistureCreate):
    time: datetime


def on_connect(client, userdata, flags, rc) -> None:
    logger.info("Connected with result code " + str(rc))
    client.subscribe("soil-moisture")


def on_message(client, userdata, msg) -> None:
    try:
        json_data = json.loads(msg.payload)
        data = SoilMoistureCreate(**json_data)
    except JSONDecodeError:
        logger.error("json parsing failed")
        return
    except ValidationError:
        logger.exception("data validation failed")
        return
    except Exception:
        logger.exception("Unexpected exception")
        return

    soil_moisture = SoilMoistureDB(time=datetime.now(timezone.utc), **data.dict())

    logger.info("adding data: %s", soil_moisture)
    soil_moisture_obj = SoilMoisture(**soil_moisture.dict())

    with SessionLocal() as session:
        session.add(soil_moisture_obj)
        session.commit()
        session.refresh(soil_moisture_obj)


def main() -> None:
    client = mqtt.Client("soil-moisture-consumer")
    client.on_connect = on_connect
    client.on_message = on_message
    client.username_pw_set(
        username=settings.MQTT_USERNAME, password=settings.MQTT_PASSWORD
    )

    try:
        client.connect(settings.MQTT_BROKER_HOSTNAME, settings.MQTT_BROKER_PORT)
    except ConnectionRefusedError:
        logger.error("Failed connectiong to mqtt server. Trying again in 3 seconds")
        time.sleep(3)
        main()

    client.loop_forever()


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        logger.error("keyboardinterrupt detected... stopping..")
