#ifndef __SHADER_PARAMETER_H__
#define __SHADER_PARAMETER_H__

#include <vector>
#include <string>

struct ShaderParameter {
  bool operator <(const ShaderParameter& rhs) const {
    return name < rhs.name;
  }
  std::string type;
  std::string name;
  bool uniform; /* else varying */
};

std::vector<ShaderParameter> parseShaderParameters(const std::string& shaderCode);

#endif // __SHADER_PARAMETER_H__
