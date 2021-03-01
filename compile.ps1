$version = '0.1'





Write-Host
Write-Host '* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *'
Write-Host "* COMPILATION OF RANDOM WALKS EMULATOR v.$version                  *"
Write-Host '*                                                             *'
Write-Host '*                          Andrei Eliseev (JointPoints), 2021 *'
Write-Host '* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *'
Write-Host





$compiler = 'g++'
$compiler_name = 'GCC'
$compiler_dumpversion_command = 'gcc -dumpversion'
$compiler_min_version = '4.8.1'

$object_folder = 'random_walks_emulator_obj'
$out_folder = "Random Walks Emulator (build, v.$version)"
$out_file = 'emulator.exe'
$units = @('metric_graph', 'wander', 'main')





# A function to check the presense of a compiler
function Check-Compiler
{
	param
	(
		[Parameter(Position = 0)]$step_no,
		[Parameter(Position = 1)]$compiler_name,
		[Parameter(Position = 2)]$compiler_verbosity_name
	)
	
	Write-Host "$step_no Looking for $compiler_verbosity_name... " -NoNewLine
	try
	{
		(Invoke-Expression $compiler_name) *> $NULL
		Write-Host '(success)'
		Write-Host
		Return $compiler_name
	}
	catch
	{
		Write-Host '(fail)'
		Return ''
	}
}





# A function to check the version of the compiler
function Check-Version
{
	param
	(
		[Parameter(Position = 0)]$step_no,
		[Parameter(Position = 1)]$compiler_dumpversion_command,
		[Parameter(Position = 2)]$min_acceptible_version
	)
	
	Write-Host "$step_no Version check... " -NoNewLine
	if ([version](Invoke-Expression $compiler_dumpversion_command) -ge [version]($min_acceptible_version))
	{
		Write-Host '(success)'
		Write-Host
		Return $true
	}
	else
	{
		Write-Host '(fail)'
		Return $false
	}
}





# A function to create object file
function Create-Object-File
{
	param
	(
		[Parameter(Position = 0)]$step_no,
		[Parameter(Position = 1)]$unit_no
	)

	Write-Host "$step_no Making object file ($($unit_no + 1)/$($units.count))... " -NoNewLine

	if ($units[$unit_no] -eq 'main')
	{
		if (Test-Path -Path 'main.cpp')
		{
			$compilation_results = (g++ -Wall -O2 -std=c++11 -c main.cpp -o "$object_folder\\main.o" 2>&1) | Out-String
			if ($compilation_results -ne '')
			{
				Write-Host '(fail)'
				Write-Host
				Write-Host '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~' -ForegroundColor Red
				Write-Host 'ERROR!' -NoNewLine -ForegroundColor Red
				Write-Host ' The following compilation error has occured:'
				Write-Host $compilation_results
				Write-Host '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~' -ForegroundColor Red
				Write-Host
				Return $false
			}
			Write-Host '(success)'
			Return $true
		}
		Write-Host '(fail)'
		Write-Host
		Write-Host '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~' -ForegroundColor Red
		Write-Host 'ERROR!' -NoNewLine -ForegroundColor Red
		Write-Host ' Some files needed for compilation are missing. Please,'
		Write-Host 'do update the contents of this folder and all its subfolders by'
		Write-Host 'downloading the source archive from the original repository. If'
		Write-Host 'you need more info about how to do it, consider visiting'
		Write-Host 'https://jointpoints.github.io/random-walks/installation.html'
		Write-Host 'The original repository may be found at'
		Write-Host 'https://github.com/jointpoints/random-walks'
		Write-Host '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~' -ForegroundColor Red
		Write-Host
		Return $false
	}

	if ((Test-Path -Path "$($units[$unit_no])\\$($units[$unit_no]).cpp") -and (Test-Path -Path "$($units[$unit_no])\\$($units[$unit_no]).hpp"))
	{
		$compilation_results = (g++ -Wall -O2 -std=c++11 -c "$($units[$unit_no])\\$($units[$unit_no]).cpp" -o "$object_folder\\$($units[$unit_no]).o" 2>&1) | Out-String
		if ($compilation_results -ne '')
		{
			Write-Host '(fail)'
			Write-Host
			Write-Host '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~' -ForegroundColor Red
			Write-Host 'ERROR!' -NoNewLine -ForegroundColor Red
			Write-Host ' The following compilation error has occured:'
			Write-Host $compilation_results
			Write-Host '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~' -ForegroundColor Red
			Write-Host
			Return $false
		}
		Write-Host '(success)'
		Return $true
	}
	Write-Host '(fail)'
	Write-Host
	Write-Host '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~' -ForegroundColor Red
	Write-Host 'ERROR!' -NoNewLine -ForegroundColor Red
	Write-Host ' Some files needed for compilation are missing. Please,'
	Write-Host 'do update the contents of this folder and all its subfolders by'
	Write-Host 'downloading the source archive from the original repository. If'
	Write-Host 'you need more info about how to do it, consider visiting'
	Write-Host 'https://jointpoints.github.io/random-walks/installation.html'
	Write-Host 'The original repository may be found at'
	Write-Host 'https://github.com/jointpoints/random-walks'
	Write-Host '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~' -ForegroundColor Red
	Write-Host
	Return $false
}





# A function to create executable
function Create-Exe
{
	param
	(
		[Parameter(Position = 0)]$step_no
	)

	Write-Host "$step_no Making a single executable... " -NoNewLine

	$compilation_results = (g++ -o ('"{0}\{1}"' -f $out_folder, $out_file) ($units | % {"$object_folder\$_.o "}) 2>&1) | Out-String
	if ($compilation_results -ne '')
	{
		Write-Host '(fail)'
		Write-Host
		Write-Host '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~' -ForegroundColor Red
		Write-Host 'ERROR!' -NoNewLine -ForegroundColor Red
		Write-Host ' The following compilation error has occured:'
		Write-Host $compilation_results
		Write-Host '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~' -ForegroundColor Red
		Write-Host
		Return $false
	}
	Write-Host '(success)'
	Return $true
}










# 1. Looking for a compiler
$compiler_found = Check-Compiler '1.' $compiler $compiler_name

if ($compiler_found -eq '')
{
	Write-Host
	Write-Host '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~' -ForegroundColor Red
	Write-Host 'ERROR!' -NoNewLine -ForegroundColor Red
	Write-Host ' No GCC compiler is available. You either do not have one'
	Write-Host 'installed, or it is not included into PATH environment variable'
	Write-Host 'of your operating system. To install GCC package on Windows you'
	Write-Host 'may visit either'
	Write-Host 'https://sourceforge.net/projects/mingw/'
	Write-Host 'or'
	Write-Host 'http://mingw-w64.org/doku.php/download/mingw-builds'
	Write-Host 'For more info about prerequisites needed to be met before'
	Write-Host 'compilation consider visiting'
	Write-Host 'https://jointpoints.github.io/random-walks/installation.html'
	Write-Host '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~' -ForegroundColor Red
	Write-Host
	Return
}





# 2. Check the version
$version_pass = Check-Version '2.' $compiler_dumpversion_command $compiler_min_version

if ($version_pass -eq $false)
{
	Write-Host
	Write-Host '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~' -ForegroundColor Red
	Write-Host 'ERROR!' -NoNewLine -ForegroundColor Red
	Write-Host ' GCC package installed on your computer is outdated and'
	Write-Host 'will not be able to compile the emulator properly. Please, do'
	Write-Host 'update it to the latest version available. Minimal requirement'
	Write-Host 'for the GCC version is 4.8.1. For more info about prerequisites'
	Write-Host 'needed to be met before compilation consider visiting'
	Write-Host 'https://jointpoints.github.io/random-walks/installation.html'
	Write-Host '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~' -ForegroundColor Red
	Write-Host
	Return
}





# 3. Compile
Write-Host '3. Compilation...'
# Object files
(New-Item -Type Directory -Force -Path $object_folder) *> $NULL
for ($unit_i = 0; $unit_i -lt $units.count; $unit_i++)
{
	if ((Create-Object-File "    3.$($unit_i + 1)" $unit_i) -eq $false)
	{
		Return
	}
}
# Linker
(New-Item -Type Directory -Force -Path "$out_folder") *> $NULL
if ((Create-Exe "    3.$($units.count + 1)") -eq $false)
{
	Return
}





# 4. Finalisation
Write-Host
Write-Host '4. Finalisation... ' -NoNewLine
# Remove object folder
(powershell Remove-Item -Force -Recurse -Path $object_folder) *> $NULL
# Make folders
(New-Item -Type Directory -Force -Path "$out_folder\My graphs") *> $NULL
(New-Item -Type Directory -Force -Path "$out_folder\My scenarios") *> $NULL
(Copy-Item -Force -Path _util\Sample1 -Destination "$out_folder\My graphs\Sample graph 1.rweg") *> $NULL
Write-Host '(success)'





Write-Host
Write-Host '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~' -ForegroundColor Green
Write-Host 'SUCCESS!' -NoNewLine -ForegroundColor Green
Write-Host ' Random Walks Emulator has been successfully compiled.'
Write-Host 'If you require any assistance with utilising this program, you'
Write-Host 'may always consult with tutorials at'
Write-Host 'https://jointpoints.github.io/random-walks/tutorials.html'
Write-Host 'You may now exit the PowerShell or run the emulator by typing'
Write-Host 'consecutively the following commands:'
Write-Host ('    Set-Location "{0}"' -f $out_folder)
Write-Host '    .\emulator.exe'
Write-Host '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~' -ForegroundColor Green
Write-Host
