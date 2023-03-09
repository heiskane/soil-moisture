from typing import Any, Optional

from pydantic import BaseSettings, PostgresDsn, validator


class Config(BaseSettings):
    LOG_LEVEL: str = "INFO"
    DEBUG: bool = False

    MQTT_BROKER_HOSTNAME: str = "192.168.1.94"
    MQTT_BROKER_PORT: int = 1883

    MQTT_USERNAME: str = "user"
    MQTT_PASSWORD: str = "pass"

    POSTGRES_SERVER: str = "database"
    POSTGRES_USER: str = "postgres"
    POSTGRES_PASSWORD: str = "postgres"
    POSTGRES_DB: str = "postgres"
    SQLALCHEMY_DATABASE_URI: Optional[PostgresDsn] = None

    # https://github.com/tiangolo/full-stack-fastapi-postgresql/blob/master/%7B%7Bcookiecutter.project_slug%7D%7D/backend/app/app/core/config.py#L42
    @validator("SQLALCHEMY_DATABASE_URI", pre=True)
    def assemble_db_connection(cls, v: Optional[str], values: dict[str, Any]) -> Any:
        if isinstance(v, str):
            return v
        return PostgresDsn.build(
            scheme="postgresql",
            user=values.get("POSTGRES_USER"),
            password=values.get("POSTGRES_PASSWORD"),
            host=str(values.get("POSTGRES_SERVER")),
            path=f"/{values.get('POSTGRES_DB') or ''}",
        )


settings = Config()
