#include "ShaderParameter.h"
#include <string.h>
#include <SymbolTable.h>
#include <ParseHelper.h>
#include <Scan.h>
#include <ScanContext.h>
#include <InitializeDll.h>
#include <stdio.h>

using namespace glslang;

class Shader : public TShader {
public:
  Shader() : TShader(EShLangFragment) {
  }
  TIntermediate* getIntermediate() {
    return intermediate;
  }
};

class VariableCollector : public TIntermTraverser {
public:

private:
  std::string stringFromType(const TType& type) {
    if (type.isArray()) {
      return "unsupported_type";
    }

    if (type.getBasicType() == EbtSampler) {
      const TSampler& sampler = type.getSampler();
      TString samplerString = sampler.getString();
      return samplerString.c_str();
    }

    if (type.getBasicType() != EbtFloat) {
      return "unsupported_type";
    }

    if (type.isScalar()) {
      return "float";
    } else if (type.isVector()) {
      return "vec" + itoa(type.getVectorSize());
    } else if (type.isMatrix()) {
      return "mat" + itoa(type.getMatrixCols()) + "x" + itoa(type.getMatrixRows());
    }
    return "unknown";
  }
  virtual void visitSymbol(TIntermSymbol* sym) {
    TQualifier qual = sym->getQualifier();
    TStorageQualifier squal = qual.storage;
    if (squal == EvqUniform) { /* || squal == EvqVaryingIn ) { */
      bool uniform = (squal == EvqUniform);
      std::string type = stringFromType(sym->getType());
      TString name = sym->getName();
      ShaderParameter var;
      var.type = type;
      var.name = name.c_str();
      var.uniform = uniform;
      vars.insert(var);
    }
  }
  std::string itoa(const int x) {
    char buffer[64];
    sprintf(buffer, "%d", x);
    return buffer;
  }
public:
  std::set<ShaderParameter> vars;
};

ShaderParameters parseShaderParameters(const std::string& shaderCode, std::string* error)
{
  if (shaderCode.empty()) {
    *error = "Error: Please supply a shader program.";
    return ShaderParameters();
  }

  Shader shader;
  const char* test[] = {"precision highp float;\n#line 1\n", shaderCode.c_str()};
  shader.setStrings(test, 2);

  TBuiltInResource resources;
  memset(&resources, 0, sizeof(resources));
  resources.limits.nonInductiveForLoops = true;

  shader.parse(&resources, 100, true, EShMsgDefault);
  //printf("%s\n", shader.getInfoLog());

  if (error) {
    *error = shader.getInfoLog();
  }

  TIntermediate* tree = shader.getIntermediate();
  TIntermNode* root = tree->getTreeRoot();

  VariableCollector collector;
  root->traverse(&collector);

  std::vector<ShaderParameter> vars(collector.vars.begin(), collector.vars.end());

  return vars;
}
