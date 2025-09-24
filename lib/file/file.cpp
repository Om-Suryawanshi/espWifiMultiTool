#include <file.h>

FileManager::FileManager()
{
}

String FileManager::listAllFiles(String path)
{
    DynamicJsonDocument doc(2048);
    FSInfo fs_info;
    LittleFS.info(fs_info);
    doc["totalBytes"] = fs_info.totalBytes;
    doc["bytesUsed"] = fs_info.usedBytes;
    JsonArray files = doc.createNestedArray("files");
    if (LittleFS.exists(path))
    {
        Dir dir = LittleFS.openDir(path);
        while (dir.next())
        {
            JsonObject file = files.createNestedObject();
            file["name"] = dir.fileName();
            file["size"] = dir.fileSize();
            if (dir.fileSize() > 0 || dir.fileName().lastIndexOf('.') > 0)
            {
                file["type"] = "file";
            }
            else
            {
                file["type"] = "dir";
            }
        }
    }
    String output;
    serializeJson(doc, output);
    return output;
}

String FileManager::getFileContent(String path)
{
    if (!LittleFS.exists(path))
    {
        return "Error: File Not Found.";
    }

    File file = LittleFS.open(path, "r");
    if (!file)
    {
        return "Error: Failed to open the file.";
    }

    String content = file.readString();
    file.close();
    return content;
}