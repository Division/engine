struct Transform {
  mat4 model;
  mat4 normalMatrix;
};

layout (std140) uniform TransformBlock {
  Transform transform;
};
