#Build and run the program

echo 'Building scanner'
gcc scanner.c -o scan.exe
echo 'Building parser'
gcc Parser.c -o parse.exe
echo 'Building Virtual Machine'
gcc vm.c -o vm.exe

echo 'Executing scanner'
./scan.exe
echo 'Executing parser'
./parse.exe
echo 'Executing Virtual Machine'
./vm.exe

echo 'Done'