#!/bin/python3

# Script validates platform JSON configuration, passed by user against provided schema.

import json
import sys
import os
from jsonschema import validate

cfg = json.load(open(sys.argv[1]))
cfg = cfg['platform']

schema = json.load(open(sys.argv[2]))

validate(cfg, schema)
