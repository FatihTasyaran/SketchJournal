##Multi Tables
python3 script_multis.py ./master_multitables 25 20 0.01 0.003 4 1 0.9 5 0.9mult_pi.csv
echo "Done!"
python3 script_multis.py ./master_multitables 25 20 0.01 0.003 4 1 1.1 5 1.1mult_pi.csv
echo "Done!"
python3 script_multis.py ./master_multitables 25 20 0.01 0.003 4 1 1.5 5 1.5mult_pi.csv
echo "Done!"
python3 script_multis.py ./master_multitables 25 20 0.01 0.003 4 1 2 5 2mult_pi.csv
echo "Done!"
python3 script_multis.py ./master_multitables 25 20 0.01 0.003 4 1 3 5 3mult_pi.csv
echo "Done!"
python3 script_multis.py ./master_multitables 25 20 0.01 0.003 4 2 0.9 2 uniform_mult_pi.csv 
echo "Done!"
echo "Multis Done!"
#Single Tables
python3 script_singles.py ./master_singletables 25 20 0.01 0.003 4 1 0.9 5 0.9single_pi.csv
echo "Done!"
python3 script_singles.py ./master_singletables 25 20 0.01 0.003 4 1 1.1 5 1.1single_pi.csv
echo "Done!"
python3 script_singles.py ./master_singletables 25 20 0.01 0.003 4 1 1.5 5 1.5single_pi.csv
echo "Done!"
python3 script_singles.py ./master_singletables 25 20 0.01 0.003 4 1 2 5 2single_pi.csv
echo "Done!"
python3 script_singles.py ./master_singletables 25 20 0.01 0.003 4 1 3 5 3single_pi.csv
echo "Done!"
python3 script_singles.py ./master_singletables 25 20 0.01 0.003 4 2 0.9 2 uniform_single_pi.csv
echo "Done!"
echo "Singles Done!"
##Padding-Barrier
python3 script_barriers.py ./master_paddingbarrier 25 20 0.01 0.003 4 1 0.9 5 0.9padbar_pi.csv
echo "Done!"
python3 script_barriers.py ./master_paddingbarrier 25 20 0.01 0.003 4 1 1.1 5 1.1padbar_pi.csv
echo "Done!"
python3 script_barriers.py ./master_paddingbarrier 25 20 0.01 0.003 4 1 1.5 5 1.5padbar_pi.csv
echo "Done!"
python3 script_barriers.py ./master_paddingbarrier 25 20 0.01 0.003 4 1 2 5 2padbar_pi.csv
echo "Done!"
python3 script_barriers.py ./master_paddingbarrier 25 20 0.01 0.003 4 1 3 5 3padbar_pi.csv
echo "Done!"
python3 script_barriers.py ./master_paddingbarrier 25 20 0.01 0.003 4 2 0.9 2 uniform_padbar_pi.csv
echo "Done!"
echo "Barrier-Padding Done!"
echo "All very well-done"
