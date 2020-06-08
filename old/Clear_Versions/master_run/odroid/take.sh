##MULTI TABLES
python3 script_multis_odroid.py ./master_multitables 25 20 0.01 0.003 8 1 0.9 5 odroid_mult_0.9.csv
echo "Done!"
python3 script_multis_odroid.py ./master_multitables 25 20 0.01 0.003 8 1 1.1 5 odroid_mult_1.1.csv
echo "Done!"
python3 script_multis_odroid.py ./master_multitables 25 20 0.01 0.003 8 1 1.5 5 odroid_mult_1.5.csv
echo "Done!"
python3 script_multis_odroid.py ./master_multitables 25 20 0.01 0.003 8 1 2 5 odroid_mult_2.csv
echo "Done!"
python3 script_multis_odroid.py ./master_multitables 25 20 0.01 0.003 8 1 3 5 odroid_mult_3.csv
echo "Done!"
python3 script_multis_odroid.py ./master_multitables 25 20 0.01 0.003 8 2 1.1 5 odroid_mult_uniform.csv
echo "Done!"
echo "Multi Tables Done!"
#SINGLE TABLES
python3 script_singles_odroid.py ./master_singletables 25 20 0.01 0.003 8 1 0.9 5 odroid_single_0.9.csv
echo "Done!"
python3 script_singles_odroid.py ./master_singletables 25 20 0.01 0.003 8 1 1.1 5 odroid_single_1.1.csv
echo "Done!"
python3 script_singles_odroid.py ./master_singletables 25 20 0.01 0.003 8 1 1.5 5 odroid_single_1.5.csv
echo "Done!"
python3 script_singles_odroid.py ./master_singletables 25 20 0.01 0.003 8 1 2 5 odroid_single_2.csv
echo "Done!"
python3 script_singles_odroid.py ./master_singletables 25 20 0.01 0.003 8 1 3 5 odroid_single_3.csv
echo "Done!"
python3 script_singles_odroid.py ./master_singletables 25 20 0.01 0.003 8 2 0.9 5 odroid_single_uniform_0.9.csv
echo "Done!"
echo "Single Tables Done!"
#PADBAR
python3 script_barriers_odroid.py ./paddingbarrier 25 20 0.01 0.003 8 1 0.9 5 odroid_padbar_0.9.csv
echo "Done!"
python3 script_barriers_odroid.py ./paddingbarrier 25 20 0.01 0.003 8 1 1.1 5 odroid_padbar_1.1.csv
echo "Done!"
python3 script_barriers_odroid.py ./paddingbarrier 25 20 0.01 0.003 8 1 1.1 5 odroid_padbar_1.1.csv
echo "Done!"
python3 script_barriers_odroid.py ./paddingbarrier 25 20 0.01 0.003 8 1 1.5 5 odroid_padbar_1.5.csv
echo "Done!"
python3 script_barriers_odroid.py ./paddingbarrier 25 20 0.01 0.003 8 1 2 5 odroid_padbar_2.csv
echo "Done!"
python3 script_barriers_odroid.py ./paddingbarrier 25 20 0.01 0.003 8 1 3 5 odroid_padbar_3.csv
echo "Done!"
python3 script_barriers_odroid.py ./paddingbarrier 25 20 0.01 0.003 8 2 0.9 5 odroid_padbar_uniform.csv
echo "PadBar Done!"
echo "All very well-done!"
