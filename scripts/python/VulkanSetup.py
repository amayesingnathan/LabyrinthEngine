import os
import sys
import subprocess
import platform
from pathlib import Path

if platform.system() == "Windows":
    import UtilsWindows as Utils
elif platform.system() == "Linux":
    import UtilsLinux as Utils

from io import BytesIO
from urllib.request import urlopen

class VulkanConfiguration:
    requiredVulkanVersion = "1.3."
    installVulkanVersion = "1.3.216.0"
    vulkanDirectory = "./Labyrinth/dependencies/VulkanSDK"

    if platform.system() == "Windows":
        vulkanPlatform = "windows"
        vulkanFilename = "vulkan_sdk.exe"
        vulkanExecPath = f"{vulkanDirectory}/VulkanSDK-{installVulkanVersion}-Installer.exe"
    elif platform.system() == "Linux":
        vulkanPlatform = "linux"
        vulkanFilename = "vulkan_sdk.tar.gz"
        vulkanExecPath = f"{vulkanDirectory}/{installVulkanVersion}/vulkan_sdk"

    @classmethod
    def Validate(cls):
        if (not cls.CheckVulkanSDK()):
            print("Vulkan SDK not installed correctly.")
            return
            
        if (not cls.CheckVulkanSDKDebugLibs()):
            print(f"\nNo Vulkan SDK debug libs found. Install Vulkan SDK with debug libs.")
            print(f"\nDebug configuration disabled.")

    @classmethod
    def CheckVulkanSDK(cls):
        vulkanSDK = os.environ.get("VULKAN_SDK")
        if (vulkanSDK is None):
            print("\nYou don't have the Vulkan SDK installed!")
            cls.__InstallVulkanSDK()
            return False
        else:
            print(f"\nLocated Vulkan SDK at {vulkanSDK}")

        if (cls.requiredVulkanVersion not in vulkanSDK):
            print(f"You don't have the correct Vulkan SDK version! (Engine requires {cls.requiredVulkanVersion})")
            cls.__InstallVulkanSDK()
            return False
    
        print(f"Correct Vulkan SDK located at {vulkanSDK}")
        return True

    @classmethod
    def __InstallVulkanSDK(cls):
        permissionGranted = False
        while not permissionGranted:
            reply = str(input("Would you like to install VulkanSDK {0:s}? [Y/N]: ".format(cls.installVulkanVersion))).lower().strip()[:1]
            if reply == 'n':
                return
            permissionGranted = (reply == 'y')

        vulkanInstallURL = f"https://sdk.lunarg.com/sdk/download/{cls.installVulkanVersion}/{cls.vulkanPlatform}/{cls.vulkanFilename}"
        vulkanInstallPath = f"{cls.vulkanDirectory}/{cls.vulkanFilename}"
        print("Downloading {0:s} to {1:s}".format(vulkanInstallURL, vulkanInstallPath))
        Utils.DownloadFile(vulkanInstallURL, vulkanInstallPath)
        print("Running Vulkan SDK installer...")        
        if platform.system() == "linux":
            print("Extracting", vulkanInstallPath)
            Utils.UnpackFile(vulkanInstallPath, [], True)
        os.startfile(os.path.abspath(vulkanExecPath))
        print("Re-run this script after installation!")
        quit()

    @classmethod
    def CheckVulkanSDKDebugLibs(cls):
        vulkanSDK = os.environ.get("VULKAN_SDK")
        shadercdLib = Path(f"{vulkanSDK}/Lib/shaderc_sharedd.lib")
        
        return shadercdLib.exists()

if __name__ == "__main__":
    VulkanConfiguration.Validate()