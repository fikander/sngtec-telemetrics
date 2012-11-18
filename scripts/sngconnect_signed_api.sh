#!/bin/bash

API_KEY=9FWqWkGpWs933kPDLD13rG65MKle87gOq42L5O32Sd1M49DQf65J0YrJCKSpThIkQ9dXfLj8g6Vw1PgAHsz4dSl6WOvKPH3kuqKZ


echo -n "/api/v1/feeds/1/datastreams.json?filter=requested:" | openssl dgst -sha256 -hmac $API_KEY

curl --header "ContentType: application/json" --header "Signature:39c885478779ccecd35e95135355d1a246413e3bb06deacd38d1fcf3aca5d1d6" server.synergiatech.pl:6543/api/v1/feeds/1/datastreams.json?filter=requested

