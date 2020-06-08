echo "Start!"

##MULTI TABLES
python3 script_multis.py ./multitables 30 25 0.01 0.003 16 1 0.9 5 multiTables_0.9.csv
echo "Done!"

python3 script_multis.py ./multitables 30 25 0.01 0.003 16 1 1.1 5 multiTables_1.1.csv
echo "Done!"

python3 script_multis.py ./multitables 30 25 0.01 0.003 16 1 1.5 5 multiTables_1.5.csv
echo "Done!"

python3 script_multis.py ./multitables 30 25 0.01 0.003 16 1 2 5 multiTables_2.csv
echo "Done!"

python3 script_multis.py ./multitables 30 25 0.01 0.003 16 1 3 5 multiTables_3.csv
echo "Done!"

python3 script_multis.py ./multitables 30 25 0.01 0.003 16 2 3 5 multiTables_uniform.csv
echo "Done!"

echo "Multi Tables Done!"


##SINGLE TABLES
python3 script_singles.py ./singletables 30 25 0.01 0.003 16 1 0.9 2 singleTables0.9.csv
echo "Done!"

python3 script_singles.py ./singletables 30 25 0.01 0.003 16 1 1.1 2 singleTables1.1.csv
echo "Done!"

python3 script_singles.py ./singletables 30 25 0.01 0.003 16 1 1.5 2 singleTables1.5.csv
echo "Done!"

python3 script_singles.py ./singletables 30 25 0.01 0.003 16 1 2 2 singleTables2.csv
echo "Done!"

python3 script_singles.py ./singletables 30 25 0.01 0.003 16 1 3 2 singleTables3.csv
echo "Done!"

python3 script_singles.py ./singletables 30 25 0.01 0.003 16 2 3 2 singleTables_uniform.csv
echo "Done!"

echo "Single Tables Done!"


##BATCH AND PADDING
python3 script_paddings.py ./batchpadding 30 25 0.01 0.003 16 1 0.9 5 batchpadding0.9.csv
echo "Done!"

python3 script_paddings.py ./batchpadding 30 25 0.01 0.003 16 1 1.1 5 batchpadding1.1.csv
echo "Done!"

python3 script_paddings.py ./batchpadding 30 25 0.01 0.003 16 1 1.5 5 batchpadding1.5.csv
echo "Done!"

python3 script_paddings.py ./batchpadding 30 25 0.01 0.003 16 1 2 5 batchpadding2.csv
echo "Done!"

python3 script_paddings.py ./batchpadding 30 25 0.01 0.003 16 1 3 5 batchpadding3.csv
echo "Done!"

python3 script_paddings.py ./batchpadding 30 25 0.01 0.003 16 2 3 5 batchpadding_uniform.csv
echo "Done!"

echo "Batch and Padding Done!"


python3 script_barriers.py ./paddingbarrier 30 25 0.01 0.003 16 1 0.9 5 barrier0.9.csv
echo "Done!"

python3 script_barriers.py ./paddingbarrier 30 25 0.01 0.003 16 1 1.1 5 barrier1.1.csv
echo "Done!"

python3 script_barriers.py ./paddingbarrier 30 25 0.01 0.003 16 1 1.5 5 barrier1.5.csv
echo "Done!"

python3 script_barriers.py ./paddingbarrier 30 25 0.01 0.003 16 1 2 5 barrier2.csv
echo "Done!"

python3 script_barriers.py ./paddingbarrier 30 25 0.01 0.003 16 1 3 5 barrier3.csv
echo "Done!"

python3 script_barriers.py ./paddingbarrier 30 25 0.01 0.003 16 2 3 5 barrier_uniform.csv
echo "Done!"

echo "Barriers Done!"

echo "All Done!"



