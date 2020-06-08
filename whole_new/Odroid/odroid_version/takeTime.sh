

python3 script_loadbalance.py ./barriers 25 20 0.01 0.003 8 1 0.9 5 odroidlbs0.9.csv
echo "Done!"

python3 script_loadbalance.py ./barriers 25 20 0.01 0.003 8 1 1.1 5 odroidlbs1.1.csv
echo "Done!"

python3 script_loadbalance.py ./barriers 25 20 0.01 0.003 8 1 1.5 5 odroidlbs1.5.csv
echo "Done!"

python3 script_loadbalance.py ./barriers 25 20 0.01 0.003 8 1 2 5 odroidlbs2.csv
echo "Done!"

python3 script_loadbalance.py ./barriers 25 20 0.01 0.003 8 1 3 5 odroidlbs3.csv
echo "Done!"

python3 script_loadbalance.py ./barriers 25 20 0.01 0.003 8 2 3 5 odroidlbsuniform.csv
echo "Done!"
