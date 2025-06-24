# =============================================================================
#         SCRIPT DE CONFIGURACIÓN AUTOMÁTICA PARA ENTORNO DE DESARROLLO
#                          PROYECTO: BELZEBUB EVOLVED
#                           VERSIÓN: 1.3 (Definitiva)
# =============================================================================
#
#   Este script instalará y configurará automáticamente todas las herramientas
#   necesarias para compilar el proyecto.
#
# =============================================================================

# --- PASO 1: VERIFICAR PRIVILEGIOS DE ADMINISTRADOR ---
function Test-Admin {
    $currentUser = New-Object Security.Principal.WindowsPrincipal $([Security.Principal.WindowsIdentity]::GetCurrent())
    return $currentUser.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
}

if (-not (Test-Admin)) {
    Write-Warning "Se requieren privilegios de Administrador. Volviendo a lanzar el script..."
    Start-Process powershell.exe -Verb RunAs -ArgumentList ('-File "{0}"' -f $MyInvocation.MyCommand.Path)
    Exit
}

Write-Host "--- Privilegios de Administrador confirmados ---" -ForegroundColor Green

# --- PASO 2: DEFINIR VARIABLES Y RUTAS ---
$BaseDir = $PSScriptRoot
$VcpkgDir = Join-Path $BaseDir "vcpkg"
$VsInstallerPath = "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vs_installer.exe"
$VswherePath = "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe"

# --- PASO 3: INSTALAR CHOCOLATEY Y GIT ---
function Install-Prerequisites {
    if (-not (Get-Command choco -ErrorAction SilentlyContinue)) {
        Write-Host "Instalando gestor de paquetes Chocolatey..." -ForegroundColor Yellow
        Set-ExecutionPolicy Bypass -Scope Process -Force
        [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
        iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
    } else { Write-Host "Chocolatey ya está instalado." -ForegroundColor Green }

    if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
        Write-Host "Instalando Git..." -ForegroundColor Yellow
        choco install git.install -y --params "/NoShellIntegration"
    } else { Write-Host "Git ya está instalado." -ForegroundColor Green }
}

# --- PASO 4: INSTALAR VISUAL STUDIO CON LÓGICA MEJORADA ---
function Install-VisualStudio {
    if (-not (Test-Path $VsInstallerPath)) {
        Write-Host "Descargando Visual Studio Installer..." -ForegroundColor Yellow
        $VsInstallerUrl = "https://aka.ms/vs/17/release/vs_Community.exe"
        $VsInstallerDownloadPath = Join-Path $env:TEMP "vs_community.exe"
        Invoke-WebRequest -Uri $VsInstallerUrl -OutFile $VsInstallerDownloadPath
        Start-Process -FilePath $VsInstallerDownloadPath -ArgumentList "--quiet", "--wait" -Wait
    }

    Write-Host "Instalando/Verificando componentes de Visual Studio (esto puede tardar mucho)..." -ForegroundColor Yellow
    
    # IDs de los componentes a instalar
    $Workload = "Microsoft.VisualStudio.Workload.NativeDesktop.Core"
    $SdkComponent = "Microsoft.VisualStudio.Component.Windows11SDK.22621"
    $WdkComponent = "Microsoft.VisualStudio.Component.WDK"
    
    # IDs para el producto Community
    $ProductId = "Microsoft.VisualStudio.Product.Community"
    $ChannelId = "VisualStudio.17.Release"

    # Determinar si VS ya está instalado
    $vsInstallPath = & $VswherePath -latest -property installationPath -products $ProductId -prerelease -nologo | Select-Object -First 1

    $commonArgs = "--quiet --norestart --add $Workload --add $SdkComponent --add $WdkComponent"
    $arguments = ""

    if ($vsInstallPath) {
        Write-Host "Visual Studio ya está instalado en: $vsInstallPath" -ForegroundColor Cyan
        Write-Host "Modificando la instalación para asegurar los componentes..."
        $arguments = "modify --installPath `"$vsInstallPath`" $commonArgs"
    } else {
        Write-Host "No se encontró Visual Studio. Realizando una nueva instalación..."
        $arguments = "install --productId $ProductId --channelId $ChannelId $commonArgs"
    }

    Write-Host "Ejecutando comando: $VsInstallerPath $arguments"
    
    $process = Start-Process $VsInstallerPath -ArgumentList $arguments -Wait -PassThru
    
    if ($process.ExitCode -ne 0) {
        throw "Visual Studio Installer falló con el código de salida: $($process.ExitCode). El script no puede continuar. Revisa los logs en %TEMP% (archivos 'dd_setup_...')."
    }
    
    Write-Host "--- Visual Studio, SDK y WDK configurados exitosamente ---" -ForegroundColor Green
}

# --- PASO 5: CONFIGURAR VCPKG E INSTALAR LIBRERÍAS ---
function Setup-Vcpkg {
    # El resto de esta función se mantiene igual, ya que su fallo era consecuencia del anterior.
    if (-not (Test-Path $VcpkgDir)) {
        Write-Host "Clonando el repositorio de vcpkg..." -ForegroundColor Yellow
        git clone https://github.com/microsoft/vcpkg.git $VcpkgDir
    } else {
        Write-Host "La carpeta de vcpkg ya existe. Actualizando..." -ForegroundColor Yellow
        Push-Location $VcpkgDir
        git pull
        Pop-Location
    }

    Write-Host "Ejecutando bootstrap de vcpkg..." -ForegroundColor Yellow
    Push-Location $VcpkgDir
    cmd /c "bootstrap-vcpkg.bat"
    Pop-Location

    Write-Host "Integrando vcpkg con Visual Studio..." -ForegroundColor Yellow
    & "$VcpkgDir\vcpkg.exe" integrate install

    Write-Host "Instalando librerías requeridas (imgui, glfw)..." -ForegroundColor Yellow
    & "$VcpkgDir\vcpkg.exe" install "imgui[core,glfw-binding,opengl3-binding]:x64-windows"
    & "$VcpkgDir\vcpkg.exe" install "glfw3:x64-windows"

    Write-Host "--- vcpkg y librerías configuradas exitosamente ---" -ForegroundColor Green
}


# --- EJECUCIÓN DEL SCRIPT ---
try {
    Install-Prerequisites
    Install-VisualStudio
    Setup-Vcpkg

    Write-Host ""
    Write-Host "============================================================" -ForegroundColor Cyan
    Write-Host "      ¡ENTORNO DE DESARROLLO CONFIGURADO EXITOSAMENTE!      " -ForegroundColor Cyan
    Write-Host "============================================================" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "Pasos siguientes:"
    Write-Host "1. Abre Visual Studio 2022."
    Write-Host "2. Crea la Solución y los Proyectos como se indica en la guía."
    Write-Host "3. ¡Ya puedes empezar a compilar!"
}
catch {
    Write-Error "Ocurrió un error durante la instalación: $_"
    exit 1
}

Read-Host "Presiona ENTER para salir..."