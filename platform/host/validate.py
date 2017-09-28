#!/bin/python3

# Script validates stm32 JSON configuration, passed by user.

import json
import sys
import os
from jsonschema import validate

cfg = json.load(open(sys.argv[1]))
cfg = cfg['platform']

schema = json.load(
    open(
        os.path.join(
            os.path.dirname(__file__), 'schemas/host.schema.json')))

validate(cfg, schema)
