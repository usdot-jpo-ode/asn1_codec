#!/bin/sh
export LD_LIBRARY_PATH=/usr/local/lib

# Start the ACM
if [ "${ACM_HTTP_SERVER}" == "true" ]; then
  echo "Starting HTTP Server"
  /build/acm -c /asn1_codec/config/${ACM_CONFIG_FILE} -v ${ACM_LOG_LEVEL} -R -H
else
  echo "Starting supervisord to run Kafka consumers"

  # Ensure number of processes is a positive number
  case "${ACM_NUMBER_OF_PROCESSES}" in
    ''|*[!0-9]*|0)
      echo "Invalid ACM_NUMBER_OF_PROCESSES: ${ACM_NUMBER_OF_PROCESSES}. Expected a positive integer." >&2
      exit 1
      ;;
  esac

  # Start supervisord
  supervisord -n -c /etc/supervisord.conf
fi

