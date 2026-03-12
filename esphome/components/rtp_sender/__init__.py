import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import microphone
from esphome.const import CONF_ID

DEPENDENCIES = ["microphone", "wifi"]
AUTO_LOAD = []

rtp_sender_ns = cg.esphome_ns.namespace("rtp_sender")
RtpSender = rtp_sender_ns.class_("RtpSender", cg.Component)

CONF_MICROPHONE_ID = "microphone_id"
CONF_HOST = "host"
CONF_PORT = "port"
CONF_PATH = "path"
CONF_SAMPLE_RATE = "sample_rate"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(RtpSender),
        cv.Required(CONF_MICROPHONE_ID): cv.use_id(microphone.Microphone),
        cv.Required(CONF_HOST): cv.string,
        cv.Optional(CONF_PORT, default=8554): cv.port,
        cv.Optional(CONF_PATH, default="/bird"): cv.string,
        cv.Optional(CONF_SAMPLE_RATE, default=48000): cv.int_range(min=8000, max=48000),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    mic = await cg.get_variable(config[CONF_MICROPHONE_ID])
    cg.add(var.set_microphone(mic))
    cg.add(var.set_host(config[CONF_HOST]))
    cg.add(var.set_port(config[CONF_PORT]))
    cg.add(var.set_path(config[CONF_PATH]))
    cg.add(var.set_sample_rate(config[CONF_SAMPLE_RATE]))
