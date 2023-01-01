import sys
import os
import subprocess
import platform

from PremakeSetup import PremakeConfiguration as PremakeRequirements
from ProjectSetup import ProjectConfiguration as ProjectConfig
from VulkanSetup import VulkanConfiguration as VulkanRequirements

def main():
    os.chdir(f"{sys.path[0]}/../..") # Change working dir to repo root
    namespaceConfigured = ProjectConfig.CheckNamespace()
    projectNameConfigured = ProjectConfig.CheckProjectName()
    premakeInstalled = PremakeRequirements.Validate()
    
    VulkanRequirements.Validate()

    if (not namespaceConfigured):
        namespace = str(input("Enter the top level name for the repo...\n")).strip()
        ProjectConfig.SetupNamespace(namespace)

    if (not projectNameConfigured):
        projectName = str(input("Enter the name for the template project...\n")).strip()
        ProjectConfig.SetupProject(projectName)

    if (premakeInstalled):
        print("\nRunning premake...")
        if platform.system() == "Windows":
            subprocess.call([os.path.abspath("./scripts/gen-projects/msvc.bat"), "nopause"])
        elif platform.system() == "Linux":
            subprocess.call([os.path.abspath("./scripts/gen-projects/gcc.sh"), "nopause"])

        print("\nSetup completed!")
    else:
        print("Labyrinth requires Premake to generate project files.")
        
        
if __name__ == "__main__":
    main()