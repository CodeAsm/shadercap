#ifndef __SHADER_PARAMETER_H__
#define __SHADER_PARAMETER_H__

#include <vector>
#include <string>

struct ShaderParameter {
  enum Bind {
    BindUnbound,
    BindGlobalTime,
    BindOutputResolution,
    BindTextureSampler,
    BindTextureResolution
  };

  Bind bind;
  std::string texture;

  std::string type;
  std::string name;
  bool uniform; /* else varying */

  bool operator <(const ShaderParameter& rhs) const {
    return name < rhs.name;
  }
};

typedef std::vector<ShaderParameter> ShaderParameters;

ShaderParameters parseShaderParameters(const std::string& shaderCode, std::string* error = 0);

#endif // __SHADER_PARAMETER_H__
