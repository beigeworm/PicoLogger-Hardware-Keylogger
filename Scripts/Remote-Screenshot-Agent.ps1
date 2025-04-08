
<# ================================== Screenshot Over Serial Agent ===============================

This script is the full version of the one-liner used in the 'Deploy Agent' function on 'Screenshots' page.
Run this manually for debugging and testing.

One-liner Script
-------------------
$q = 10;$s = 2;Add-Type -AssemblyName System.Windows.Forms;Add-Type -AssemblyName System.Drawing;Add-Type -TypeDefinition "using System;using System.Runtime.InteropServices;using System.Drawing;using System.Drawing.Imaging;public class ScreenCapture{[DllImport(`"user32.dll`")]public static extern IntPtr GetWindowDC(IntPtr hWnd);[DllImport(`"user32.dll`")]public static extern int ReleaseDC(IntPtr hWnd, IntPtr hDC);[DllImport(`"gdi32.dll`")]public static extern int BitBlt(IntPtr hdcDest,int nXDest,int nYDest,int nWidth,int nHeight,IntPtr hdcSrc,int nXSrc,int nYSrc,int dwRop);public static void CaptureFullScreen(string filePath,int width,int height){Bitmap bitmap=new Bitmap(width, height);Graphics g=Graphics.FromImage(bitmap);IntPtr hdcDest=g.GetHdc();IntPtr hdcSrc=GetWindowDC(IntPtr.Zero);BitBlt(hdcDest, 0, 0, width, height, hdcSrc, 0, 0, 0x00CC0020);ReleaseDC(IntPtr.Zero, hdcSrc);g.ReleaseHdc(hdcDest);Bitmap resizedBitmap=new Bitmap(bitmap,new Size(bitmap.Width/$s,bitmap.Height/$s));SaveJpegWithQuality(resizedBitmap,filePath,$q);bitmap.Dispose();resizedBitmap.Dispose();g.Dispose();}private static void SaveJpegWithQuality(Bitmap bitmap,string path,long quality){ImageCodecInfo jpegCodec=GetEncoderInfo(`"image/jpeg`");Encoder qualityEncoder=Encoder.Quality;EncoderParameters encoderParams=new EncoderParameters(1);EncoderParameter qualityParam=new EncoderParameter(qualityEncoder, quality);encoderParams.Param[0]=qualityParam;bitmap.Save(path,jpegCodec,encoderParams);}private static ImageCodecInfo GetEncoderInfo(string mimeType){ImageCodecInfo[] codecs=ImageCodecInfo.GetImageEncoders();foreach(ImageCodecInfo codec in codecs){if(codec.MimeType == mimeType){return codec;}}return null;}}" -ReferencedAssemblies 'System.Drawing','System.Windows.Forms';function Snap{$f = "$env:TEMP\screenshot.jpg";$d = [System.Windows.Forms.Screen]::PrimaryScreen;[ScreenCapture]::CaptureFullScreen($f,$d.Bounds.Width,$d.Bounds.Height);$b = [Convert]::ToBase64String([System.IO.File]::ReadAllBytes($f));$l = $b.Length;$p.WriteLine("[SIZE]$l");$p.WriteLine("[BEGIN]");$c = 512;for($i = 0;$i -lt $b.Length;$i += $c){$z=$b.Substring($i,[Math]::Min($c,$b.Length - $i));$p.WriteLine("[CHUNK]$z")}$p.WriteLine("[END]");Write-Host "$l : Sent."}function Com{$ps=Get-WMIObject Win32_SerialPort | Where-Object{$_.PNPDeviceID -match "VID_239A"};return $ps.DeviceID}$x = Com;$p = New-Object System.IO.Ports.SerialPort $x,115200,None,8,one;$p.DtrEnable = $true;$p.RtsEnable = $true;$p.Open();while($true){if($p.BytesToRead -gt 0){$y = $p.ReadLine().Trim();if($y -eq "take screenshot"){Snap}}Start-Sleep -M 200}
#>


$q = 20
$s = 2

Add-Type -AssemblyName System.Windows.Forms
Add-Type -AssemblyName System.Drawing

# Load the necessary assemblies
Add-Type -TypeDefinition @"
using System;
using System.Runtime.InteropServices;
using System.Drawing;
using System.Drawing.Imaging;

public class ScreenCapture {
    [DllImport("user32.dll")]
    public static extern IntPtr GetWindowDC(IntPtr hWnd);

    [DllImport("user32.dll")]
    public static extern int ReleaseDC(IntPtr hWnd, IntPtr hDC);

    [DllImport("gdi32.dll")]
    public static extern int BitBlt(IntPtr hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, IntPtr hdcSrc, int nXSrc, int nYSrc, int dwRop);

    public static void CaptureFullScreen(string filePath, int width, int height) {
        Bitmap bitmap = new Bitmap(width, height);
        Graphics g = Graphics.FromImage(bitmap);
        IntPtr hdcDest = g.GetHdc();
        IntPtr hdcSrc = GetWindowDC(IntPtr.Zero);
        BitBlt(hdcDest, 0, 0, width, height, hdcSrc, 0, 0, 0x00CC0020);
        ReleaseDC(IntPtr.Zero, hdcSrc);
        g.ReleaseHdc(hdcDest);
        Bitmap resizedBitmap = new Bitmap(bitmap, new Size(bitmap.Width / $s, bitmap.Height / $s));
        SaveJpegWithQuality(resizedBitmap, filePath, $q);
        bitmap.Dispose();
        resizedBitmap.Dispose();
        g.Dispose();
    }

    private static void SaveJpegWithQuality(Bitmap bitmap, string path, long quality) {
        ImageCodecInfo jpegCodec = GetEncoderInfo("image/jpeg");
        Encoder qualityEncoder = Encoder.Quality;
        EncoderParameters encoderParams = new EncoderParameters(1);
        EncoderParameter qualityParam = new EncoderParameter(qualityEncoder, quality);
        encoderParams.Param[0] = qualityParam;
        bitmap.Save(path, jpegCodec, encoderParams);
    }

    private static ImageCodecInfo GetEncoderInfo(string mimeType) {
        ImageCodecInfo[] codecs = ImageCodecInfo.GetImageEncoders();
        foreach (ImageCodecInfo codec in codecs) {
            if (codec.MimeType == mimeType) {
                return codec;
            }
        }
        return null;
    }
}
"@ -ReferencedAssemblies 'System.Drawing', 'System.Windows.Forms'


function Take-Screenshot {

    $screenshotPath = "$env:TEMP\screenshot.jpg"
    $screen = [System.Windows.Forms.Screen]::PrimaryScreen
    $Width = $screen.Bounds.Width
    $Height = $screen.Bounds.Height

    [ScreenCapture]::CaptureFullScreen($screenshotPath, $width, $height)

    Write-Host "Screenshot saved as JPEG: $screenshotPath"

    $base64Image = [Convert]::ToBase64String([System.IO.File]::ReadAllBytes($screenshotPath))
    $totalLength = $base64Image.Length
    Write-Host "Sending Base64 image: $totalLength bytes"

    $port.WriteLine("[SIZE]$totalLength")

    $port.WriteLine("[BEGIN]")

    $chunkSize = 512
    for ($i = 0; $i -lt $base64Image.Length; $i += $chunkSize) {
        $chunk = $base64Image.Substring($i, [Math]::Min($chunkSize, $base64Image.Length - $i))
        $port.WriteLine("[CHUNK]$chunk")
    }

    $port.WriteLine("[END]")
    Write-Host "Sent."
}


function Find-COMPort {
    $picoVID = "239A"
    $ports = Get-WMIObject Win32_SerialPort | Where-Object { $_.PNPDeviceID -match "VID_$picoVID" }
    return $ports.DeviceID
}

$COM_PORT = Find-COMPort
$BAUD_RATE = 115200
$port = New-Object System.IO.Ports.SerialPort $COM_PORT, $BAUD_RATE, None, 8, one
$port.DtrEnable = $true
$port.RtsEnable = $true
$port.Open()

while ($true) {
    if ($port.BytesToRead -gt 0) {
        $command = $port.ReadLine().Trim()
        if ($command -eq "take screenshot") {
            Take-Screenshot
        }
    }
    Start-Sleep -Milliseconds 200
}
