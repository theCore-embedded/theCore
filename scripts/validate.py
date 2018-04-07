#!/bin/python3

# Script validates JSON configuration, passed by user against provided schema.
# Subobject name is passed by the last argument.

import json
import sys
import os
from jsonschema import validate, RefResolver

cfg = json.load(open(sys.argv[1]))
schema = json.load(open(sys.argv[2]))
# this is a directory name (root) where the base schema is located
schema_path = 'file:///{0}/'.format(
      os.path.dirname(sys.argv[2]).replace("\\", "/"))

if len(sys.argv) > 3:
    cfg = cfg[sys.argv[3]]

resolver = RefResolver(schema_path, schema)

validate(cfg, schema, resolver=resolver)
