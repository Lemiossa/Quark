#!/bin/bash

valid_isrs=$1

if [ -z "${valid_isrs}" ]; then
	exit 1;
fi

if ! [[ "$valid_isrs" =~ ^[0-9]+$ ]]; then
	echo "numero invalido"
	exit 1
fi

printf "/**\n"
printf " * int_table.S\n"
printf " * Criado por Matheus Leme Da Silva\n"
printf " */\n"
printf ".code32\n"

for ((i=0;i<valid_isrs;i++)); do
	printf ".extern  isr_${i}\n"
done

printf ".global isr_count\nisr_count: .byte ${valid_isrs}\n\n"
printf ".global isr_table\nisr_table:\n"
for ((i=0;i<valid_isrs;i++)); do
	printf "\t.long isr_${i}\n"
done

