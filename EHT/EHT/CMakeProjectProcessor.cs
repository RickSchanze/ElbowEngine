namespace EHT;

public struct ProjectInfo
{
    public string SourceDirectory;
    public string IncludeDirectory;
}

public class ProjectMap
{
    Dictionary<string, ProjectInfo> _projects = new Dictionary<string, ProjectInfo>();
    public string RootDirectory { get; set; } = string.Empty;

    public bool TryGetProjectInfo(string projectName, out ProjectInfo projectInfo)
    {
        return _projects.TryGetValue(projectName, out projectInfo);
    }

    public void AddProjectInfo(string projectName, ProjectInfo projectInfo)
    {
        _projects.Add(projectName, projectInfo);
    }

    public bool TryGetFileProjectInfo(string filePath, out ProjectInfo projectInfo)
    {
        filePath = filePath.Replace("\\", "/");
        foreach (var (projPath, info) in _projects)
        {
            string judgeString = "/" + projPath + "/";
            if (filePath.Contains(judgeString))
            {
                projectInfo = info;
                return true;
            }
        }
        projectInfo = default;
        return false;
    }
}

public static class CMakeProjectProcessor
{
    public static List<string> FindCMakeProjects(string directory)
    {
        string[] allCMakes = Directory.GetFiles(directory, "CMakeLists.txt", SearchOption.AllDirectories);
        List<string> folders = new();
        foreach (string file in allCMakes)
        {
            string parent = file.Substring(directory.Length + 1);
            if (!parent.StartsWith("CMakeLists.txt"))
            {
                string end = Path.DirectorySeparatorChar + "CMakeLists.txt";
                if (end.Length == parent.Length)
                    continue;
                string result = parent.Substring(0, parent.Length - end.Length);
                folders.Add(result);
            }
        }

        return folders;
    }

    public static ProjectMap CreateFolders(string outputPath, List<string> folders)
    {
        ProjectMap projectMap = new();
        projectMap.RootDirectory = outputPath;
        string generatedFolder = Path.Combine(outputPath, "Generated");
        Directory.CreateDirectory(generatedFolder);
        string includeFolder = Path.Combine(generatedFolder, "Include");
        string sourceFolder = Path.Combine(generatedFolder, "Source");
        Directory.CreateDirectory(includeFolder);
        foreach (string folder in folders)
        {
            ProjectInfo info;
            info.IncludeDirectory = Path.Combine(includeFolder, folder);
            info.SourceDirectory = Path.Combine(sourceFolder, folder);
            Directory.CreateDirectory(info.IncludeDirectory);
            Directory.CreateDirectory(Path.Combine(info.SourceDirectory));
            projectMap.AddProjectInfo(folder, info);
        }

        return projectMap;
    }
}