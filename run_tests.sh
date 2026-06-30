#!/bin/bash

rm -f data/Bank_Data.json
rm -f data/setting.json
rm -f data/Users.json

CPP_FILE="user.cpp"
BINARY="./user"

echo "Compiling $CPP_FILE..."
g++ -O3 "$CPP_FILE" -o "$BINARY"

if [ $? -ne 0 ]; then
    echo "Compilation failed!"
    exit 1
fi

echo "Compilation successful. Running tests..."
echo "========================================="

echo "Memory Usage Report (Command used: ./run_tests.sh)" > memory_report.txt
echo "=========================================" >> memory_report.txt
printf "%-20s | %-15s\n" "Input File" "Max RAM (KB)" >> memory_report.txt
printf "%-20s | %-15s\n" "--------------------" "---------------" >> memory_report.txt

for input_file in big_tests/*.in; do
    [ -e "$input_file" ] || continue
    base_name=$(basename "$input_file" .in)
    output_file="big_tests/${base_name}.out"
    /usr/bin/time -v "$BINARY" < "$input_file" > "$output_file" 2> temp_time.txt    
    mem_kb=$(grep "Maximum resident set size" temp_time.txt | awk '{print $6}')
    echo "Processed $input_file -> $output_file | RAM: ${mem_kb} KB"
    printf "%-20s | %-15s KB\n" "$input_file" "$mem_kb" >> memory_report.txt
done

rm -f temp_time.txt
rm -f data/Bank_Data.json
rm -f data/setting.json
rm -f data/Users.json

echo "========================================="
echo "Done! Report saved in memory_report.txt"

rm -f data/Bank_Data.json
