$myRoot = $PSScriptRoot
$AX_ROOT = $myRoot

$build1kPath = Join-Path $myRoot '1k/build1k.ps1'
$prefix = Join-Path $myRoot 'tools/external'
if (!(Test-Path $prefix -PathType Container)) {
    mkdir $prefix | Out-Null
}

# setup toolchains: glslcc, cmake, ninja, ndk, jdk, ...
. $build1kPath -setupOnly $true -prefix $prefix

# https://learn.microsoft.com/en-us/powershell/module/microsoft.powershell.core/about/about_environment_variables
$IsWin = $IsWindows -or ("$env:OS" -eq 'Windows_NT')

if ($IsLinux) {
    Write-Host "Are you continue install linux dependencies for axmol? (y/n) " -NoNewline
    $answer = Read-Host
    if ($answer -like 'y*') {
        b1k_print "It will take few minutes"
        sudo apt update
        # for vm, libxxf86vm-dev also required

        $DEPENDS = @()

        $DEPENDS += 'libx11-dev'
        $DEPENDS += 'automake'
        $DEPENDS += 'libtool'
        $DEPENDS += 'cmake'
        $DEPENDS += 'libxmu-dev'
        $DEPENDS += 'libglu1-mesa-dev'
        $DEPENDS += 'libgl2ps-dev'
        $DEPENDS += 'libxi-dev'
        $DEPENDS += 'libzip-dev'
        $DEPENDS += 'libpng-dev'
        $DEPENDS += 'libfontconfig1-dev'
        $DEPENDS += 'libgtk-3-dev'
        $DEPENDS += 'binutils'
        $DEPENDS += 'libbsd-dev'
        $DEPENDS += 'libasound2-dev'
        $DEPENDS += 'libxxf86vm-dev'
        $DEPENDS += 'libvlc-dev', 'libvlccore-dev', 'vlc'

        # if vlc encouter codec error, install
        # sudo apt install ubuntu-restricted-extras

        sudo apt install --allow-unauthenticated --yes $DEPENDS > /dev/null

        b1k_print "Installing latest freetype for linux ..."
        mkdir buildsrc
        Set-Location buildsrc
        git clone 'https://github.com/freetype/freetype.git'
        Set-Location freetype
        git checkout 'VER-2-13-0'
        sh autogen.sh
        ./configure '--prefix=/usr' '--enable-freetype-config' '--disable-static'
        sudo make install
        Set-Location ..
        Set-Location ..
    }
}

if ($IsWin) {
    $myProcess = [System.Diagnostics.Process]::GetCurrentProcess()
    $parentProcess = $myProcess.Parent
    if (!$parentProcess) {
        $myPID = $myProcess.Id
        $instance = Get-WmiObject Win32_Process -Filter "ProcessId = $myPID"
        $parentProcess = Get-Process -Id $instance.ParentProcessID
    }
    $parentProcessName = $parentProcess.ProcessName
    if ($parentProcessName -like "explorer") {
        b1k_print "setup successfully, press any key to exit . . ." -NoNewline
        cmd /c pause 1>$null
        exit 0
    }
}

b1k_print 'setup successfully.'

