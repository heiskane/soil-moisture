import json
import logging
import time
from datetime import datetime
from json import JSONDecodeError

import paho.mqtt.client as mqtt
from pydantic import BaseModel, BaseSettings, ValidationError


class Config(BaseSettings):
    LOG_LEVEL: str = "INFO"

    MQTT_BROKER_HOSTNAME: str = "127.0.0.1"
    MQTT_BROKER_PORT: int = 1883

    MQTT_USERNAME: str = "user"
    MQTT_PASSWORD: str = "pass"


settings = Config()

logging.basicConfig(format="%(levelname)s: %(asctime)s %(message)s")
logger = logging.getLogger()
logger.setLevel(settings.LOG_LEVEL)


class SoilMoisture(BaseModel):
    sensor: str
    value: int


class SoilMoistureDB(SoilMoisture):
    time: datetime


def on_connect(client, userdata, flags, rc) -> None:
    logger.info("Connected with result code " + str(rc))
    client.subscribe("soil-moisture")


def on_message(client, userdata, msg) -> None:
    try:
        json_data = json.loads(msg.payload)
        data = SoilMoisture(**json_data)
    except JSONDecodeError:
        logger.error("json parsing failed")
        return
    except ValidationError:
        logger.exception("data validation failed")
        return

    logger.info(data)


def main() -> None:
    client = mqtt.Client()
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
    main()
