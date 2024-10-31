#!/bin/bash

# Numărul de cereri de trimis
NUM_REQUESTS=500

# Trimitem cereri în paralel folosind curl
for ((i=1; i<=NUM_REQUESTS; i++)); do
  curl http://localhost:8080/ &
done

# Așteaptă ca toate procesele să se termine
wait