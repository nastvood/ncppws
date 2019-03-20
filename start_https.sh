#!/bin/bash

SERVER="http-server"

NPM_CHECK=`npm list -g --depth 0 | grep " ${SERVER}@"` 

if [ "${NPM_CHECK}" == "" ]; then
  npm install -g ${SERVER}
fi

${SERVER} -S -C localhost.crt -K localhost.key -p 8081
