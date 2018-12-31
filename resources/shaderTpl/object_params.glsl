struct Transform {
  mat4 model;
  mat4 normalMatrix;
  uint layer;
};

layout (std140) uniform TransformBlock {
  Transform transform;
};
