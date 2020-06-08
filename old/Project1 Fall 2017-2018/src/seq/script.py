import subprocess
print  "USING NORMAL DISTRIBUTION SINGLE THREAD EPSILON 0.01 NON MERGED"
for i in range(10):
	subprocess.call(["./pis", "30","25","0.01"," 0.003"," 1"," 1","0"])
print  "USING UNIFORM DISTRIBUTION SINGLE THREAD EPSILON 0.01 NON MERGED"
for i in range(10):
	subprocess.call(["./pis", "30","25","0.01"," 0.003"," 1"," 2","0"])
