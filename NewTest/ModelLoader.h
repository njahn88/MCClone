#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

enum Model {
    Box
};

struct ModelLoader {
private:
    std::string m_modelFolderPath = "";

    std::vector<float> GetModelData(std::string modelPath) {
        std::vector<float> modelData{};

        std::vector<float> modelVerts{};
        std::vector<float> modelVertexTextureCoords{};
        std::vector<float> modelNormals{};

        std::string completeModelPath = m_modelFolderPath + modelPath + ".obj";
        std::ifstream file(completeModelPath);

        if (!file) {
            std::cerr << "File not found\n" << std::endl;
            return {};
        }

        std::string line;

        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;

            if (prefix == "v") {
                float x, y, z;
                iss >> x >> y >> z;
                modelVerts.push_back(x);
                modelVerts.push_back(y);
                modelVerts.push_back(z);
            }

            if (prefix == "vn") {
                float x, y, z;
                iss >> x >> y >> z;
                modelNormals.push_back(x);
                modelNormals.push_back(y);
                modelNormals.push_back(z);
            }

            if (prefix == "vt") {
                float x, y;
                iss >> x >> y;
                modelVertexTextureCoords.push_back(x);
                modelVertexTextureCoords.push_back(y);
            }

            if (prefix == "f") {
                std::string group;
                while (iss >> group) {
                    std::istringstream gs(group);
                    std::string vStr, vtStr, vnStr;

                    std::getline(gs, vStr, '/');
                    std::getline(gs, vtStr, '/');
                    std::getline(gs, vnStr, '/');

                    int vertIndex = (std::stoi(vStr) - 1) * 3;
                    int textureIndex = (std::stoi(vtStr) - 1) * 2;
                    int normalIndex = (std::stoi(vnStr) - 1) * 3;

                    modelData.push_back(modelVerts[vertIndex]);
                    modelData.push_back(modelVerts[vertIndex + 1]);
                    modelData.push_back(modelVerts[vertIndex + 2]);

                    modelData.push_back(modelVertexTextureCoords[textureIndex]);
                    modelData.push_back(modelVertexTextureCoords[textureIndex + 1]);

                    modelData.push_back(modelNormals[normalIndex]);
                    modelData.push_back(modelNormals[normalIndex + 1]);
                    modelData.push_back(modelNormals[normalIndex + 2]);
                }

            }
        }

        return modelData;
    }

public:
    ModelLoader(std::string modelFolderPath) : m_modelFolderPath(modelFolderPath) {}

    std::vector<float> GetModel(Model model) {
        switch (model) {
        case Model::Box:
            return GetModelData("Testing");
            break;
        }
    }
};
