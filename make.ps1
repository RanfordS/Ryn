# $ for variable, `$ for literal dollar
$dest = "Ryn.exe"
$flags = "-fstack-protector-all", "-fcf-protection=full", "-fverbose-asm", "-Wall", "-Wextra", "-Wno-unused-parameter"

Write-Host "Running build"
$basepath = (Get-Location).Path
$baselen = ($basepath | Measure-Object -Character).Characters
Write-Host "Base path is $basepath"
Write-Host "Base len is $baselen"

$objs = New-Object Collections.Generic.List[String]

foreach ($child in (Get-ChildItem -Recurse -Path .\src -Filter *.c).FullName)
{
    $src = ".\" + $child.Substring($baselen + 1)
    $obj = ".\obj" + $src.Substring(5)
    $obj = $obj.Substring(0,($obj | Measure-Object -Character).Characters - 1) + "o"

    Write-Host "Compiling: $src to $obj"
    $objs.Add($obj)
    gcc $flags -o $obj -c $src
}

Write-Host "Building $dest"
gcc $flags -o $dest $objs

Write-Host "Done"
