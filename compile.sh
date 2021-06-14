#!/bin/bash
version='0.2'





echo
echo '* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *'
echo "* COMPILATION OF RANDOM WALKS EMULATOR v.$version                  *"
echo '*                                                             *'
echo '*                          Andrei Eliseev (JointPoints), 2021 *'
echo '* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *'
echo





compiler='g++'
compiler_name='GCC'
compiler_dumpversion_command='gcc -dumpversion'
compiler_min_version='4.8.1'

object_folder='random_walks_emulator_obj'
out_folder="Random Walks Emulator (build, v.$version)"
out_file='rwe'
units=('metric_graph' 'rw_space' 'ui' 'main')





# A function to check the presense of a compiler
function Check-Compiler {
: '	param
	(
		[Parameter(Position = 0)]$step_no,
		[Parameter(Position = 1)]$compiler_name,
		[Parameter(Position = 2)]$compiler_verbosity_name
	)'
	
	echo -n "$1 Looking for $3... "
	{
		(command -v $2 &>/dev/null) &&
		echo '(success)' &&
		echo &&
		return 0
	} || {
		echo '(fail)'
		return 1
	}
}





# A function to check the version of the compiler
function Check-Version {
: '	param
	(
		[Parameter(Position = 0)]$step_no,
		[Parameter(Position = 1)]$compiler_dumpversion_command,
		[Parameter(Position = 2)]$min_acceptible_version
	)'
	
	echo -n "$1 Version check... "
	if [ "$(printf '%s\n' "$3" "$(eval $2)" | sort -V | head -n1)" = "$3" ]; then
		echo '(success)'
		echo
		return 0
	else
		echo '(fail)'
		return 1
	fi
}





# A function to create object file
function Create-Object-File {
: '	param
	(
		[Parameter(Position = 0)]$step_no,
		[Parameter(Position = 1)]$unit_no
	)'

	echo -n "$1 Making object file ($(($2 + 1))/${#units[@]})... "

	if [ ${units[$2]} = 'main' ]; then
		if [ -f 'main.cpp' ]; then
			compilation_results=$(g++ -Wall -O2 -std=c++11 -c main.cpp -o "$object_folder/main.o" 2>&1)
			if (( $compilation_results -ne '' )); then
				echo '(fail)'
				echo
				echo -e '\033[0;31m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m'
				echo -e -n '\033[0;31mERROR!\033[0m'
				echo ' The following compilation error has occured:'
				echo $compilation_results
				echo -e '\033[0;31m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m'
				echo
				return 1
			fi
			echo '(success)'
			return 0
		fi
		echo '(fail)'
		echo
		echo -e '\033[0;31m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m'
		echo -e -n '\033[0;31mERROR!\033[0m'
		echo ' Some files needed for compilation are missing. Please,'
		echo 'do update the contents of this folder and all its subfolders by'
		echo 'downloading the source archive from the original repository. If'
		echo 'you need more info about how to do it, consider visiting'
		echo 'https://jointpoints.github.io/random-walks/installation.html'
		echo 'The original repository may be found at'
		echo 'https://github.com/jointpoints/random-walks'
		echo -e '\033[0;31m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m'
		echo
		return 1
	fi

	if ( [ -f "${units[$2]}/${units[$2]}.cpp" ] && [ -f "${units[$2]}/${units[$2]}.hpp" ] ); then
		compilation_results=$(g++ -Wall -O2 -std=c++11 -c "${units[$2]}/${units[$2]}.cpp" -o "$object_folder/${units[$2]}.o" 2>&1)
		if (( $compilation_results -ne '' )); then
			echo '(fail)'
			echo
			echo -e '\033[0;31m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m'
			echo -e -n '\033[0;31mERROR!\033[0m'
			echo ' The following compilation error has occured:'
			echo $compilation_results
			echo -e '\033[0;31m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m'
			echo
			return 1
		fi
		echo '(success)'
		return 0
	fi
	echo '(fail)'
	echo
	echo -e '\033[0;31m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m'
	echo -e -n '\033[0;31mERROR!\033[0m'
	echo ' Some files needed for compilation are missing. Please,'
	echo 'do update the contents of this folder and all its subfolders by'
	echo 'downloading the source archive from the original repository. If'
	echo 'you need more info about how to do it, consider visiting'
	echo 'https://jointpoints.github.io/random-walks/installation.html'
	echo 'The original repository may be found at'
	echo 'https://github.com/jointpoints/random-walks'
	echo -e '\033[0;31m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m'
	echo
	return 1
}





# A function to create executable
function Create-Exe {
: '	param
	(
		[Parameter(Position = 0)]$step_no
	)'

	echo -n "$1 Making a single executable... "

	obj_files_list=$(printf "$object_folder/%s.o " "${units[@]}")
	compilation_results=$(g++ -static -o "$out_folder/$out_file" $obj_files_list 2>&1)
	if (( $compilation_results -ne '' )); then
		echo '(fail)'
		echo
		echo -e '\033[0;31m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m'
		echo -e -n '\033[0;31mERROR!\033[0m'
		echo ' The following compilation error has occured:'
		echo $compilation_results
		echo -e '\033[0;31m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m'
		echo
		return 1
	fi
	echo '(success)'
	return 0
}










# 1. Looking for a compiler
Check-Compiler '1.' $compiler $compiler_name
compiler_found=$?

if [ $compiler_found -eq 1 ]; then
	echo
	echo -e '\033[0;31m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m'
	echo -e -n '\033[0;31mERROR!\033[0m'
	echo ' No GCC compiler is available. You either do not have one'
	echo 'installed, or it is not included into PATH environment variable'
	echo 'of your operating system. To install GCC package on Unix you'
	echo 'may run'
	echo '    sudo apt install g++'
	echo 'For more info about prerequisites needed to be met before'
	echo 'compilation consider visiting'
	echo 'https://jointpoints.github.io/random-walks/installation.html'
	echo -e '\033[0;31m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m'
	echo
	exit
fi





# 2. Check the version
Check-Version '2.' "$compiler_dumpversion_command" $compiler_min_version
version_pass=$?

if [ $version_pass -eq 1 ]; then
	echo
	echo -e '\033[0;31m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m'
	echo -e -n '\033[0;31mERROR!\033[0m'
	echo ' GCC package installed on your computer is outdated and'
	echo 'will not be able to compile the emulator properly. Please, do'
	echo 'update it to the latest version available. Minimal requirement'
	echo 'for the GCC version is 4.8.1. For more info about prerequisites'
	echo 'needed to be met before compilation consider visiting'
	echo 'https://jointpoints.github.io/random-walks/installation.html'
	echo -e '\033[0;31m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m'
	echo
	exit
fi





# 3. Compile
echo '3. Compilation...'
# Object files
mkdir -p $object_folder
for unit_i in `seq 0 $((${#units[@]} - 1))`
do
	Create-Object-File "    3.$(($unit_i + 1))" $unit_i
	if [ $? -eq 1 ]; then
		exit
	fi
done
# Linker
mkdir -p "$out_folder"
Create-Exe "    3.$((${#units[@]} + 1))"
if [ $? -eq 1 ]; then
	exit
fi





# 4. Finalisation
echo
echo -n '4. Finalisation... '
# Remove object folder
rm -rf $object_folder
# Make folders
mkdir -p "$out_folder/My graphs"
mkdir -p "$out_folder/My scenarios"
mkdir -p "$out_folder/Technical files"
cp _util/gs1 "$out_folder/My graphs/Sample graph 1.rweg"
cp _util/ss1 "$out_folder/My scenarios/Basic.rwes"
cp _util/tfrm "$out_folder/Technical files/ReadMe.txt"
cp _util/cmdgc "$out_folder/Technical files/cmdgc"
cp _util/cmdh "$out_folder/Technical files/cmdh"
cp _util/cmdr "$out_folder/Technical files/cmdr"
cp _util/cmdrtg "$out_folder/Technical files/cmdrtg"
cp _util/cmdsc "$out_folder/Technical files/cmdsc"
echo '(success)'





echo
echo -e '\033[0;32m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m'
echo -e -n '\033[0;32mSUCCESS!\033[0m'
echo ' Random Walks Emulator has been successfully compiled.'
echo 'If you require any assistance with utilising this program, you'
echo 'may always consult with tutorials at'
echo 'https://jointpoints.github.io/random-walks/tutorials.html'
echo 'You may now exit the shell or run the emulator by typing'
echo 'consecutively the following commands:'
echo "    cd \"$out_folder\""
echo '    ./rwe'
echo -e '\033[0;32m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m'
echo
