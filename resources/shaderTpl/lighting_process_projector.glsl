  for (uint i = 0u; i < projectorCount; i++) {
    uint currentOffset = lightOffset + i + pointLightCount + spotLightCount;
    {% if USE_BUFFER_TEXTURE %}
        uint projectorIndex = texelFetch(uLightIndices, int(currentOffset)).r;
    {% else %}
        uint projectorIndex = texelFetch(uLightIndices, ivec2(currentOffset % 4096u, int(floor(float(currentOffset) / 4096.0))), 0).r;
    {% endif %}

    vec4 projectedTextureUV = projectors[projectorIndex].projectionMatrix * vPosition_worldspace;
    projectedTextureUV /= projectedTextureUV.w;
    projectedTextureUV = (projectedTextureUV + 1.0) / 2.0;
    if (projectedTextureUV.x >= 0.0 && projectedTextureUV.x < 1.0
        && projectedTextureUV.y >= 0.0 && projectedTextureUV.y < 1.0
        && projectedTextureUV.z >= 0.0 && projectedTextureUV.z < 1.0) {
      vec2 spritesheetUV = projectedTextureUV.xy * projectors[projectorIndex].scale + projectors[projectorIndex].offset;
      vec4 projectedTexture = texture(uProjectorTexture, spritesheetUV) * projectors[projectorIndex].color;

      vec3 lightPosition = projectors[projectorIndex].position;
      float linearAttenuation = projectors[projectorIndex].linearAttenuation;
      float squareAttenuation = projectors[projectorIndex].squareAttenuation;

      vec3 lightDir = vPosition_worldspace.xyz - lightPosition;
      float distanceToLight = length(lightDir);
      lightDir /= distanceToLight; // normalize
      vec3 lightColor = projectedTexture.rgb * projectedTexture.a;
      vec3 lightValue = calculateFragmentDiffuse(distanceToLight, linearAttenuation, squareAttenuation, normal_worldspace, lightDir, eyeDir_worldspace, lightColor, materialSpecular);

      if (projectors[projectorIndex].shadowmapScale.x > 0) {
        vec2 shadowmapUV = projectedTextureUV.xy * projectors[projectorIndex].shadowmapScale + projectors[projectorIndex].shadowmapOffset;
        float shadow = calculateFragmentShadow(shadowmapUV, projectedTextureUV.z);
        lightValue *= 1.0 - shadow;
      }

      lightsColor += vec4(lightValue, 0.0);
    }
  }