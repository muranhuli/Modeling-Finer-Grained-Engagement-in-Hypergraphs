g++ kh-core.cpp -o kh-core
./kh-core
source ~/anaconda3/etc/profile.d/conda.sh
conda activate tools
cd
python send.py
conda deactivate