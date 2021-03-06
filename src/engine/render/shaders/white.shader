[vertex]

attribute vec3 aPosition;

uniform mat4 uMVMatrix;
uniform mat4 uPMatrix;

void main(void) {
  gl_PointSize = 5.0;
  gl_Position = uPMatrix * uMVMatrix * vec4(aPosition, 1.0);
}

[fragment]

precision mediump float;

void main(void) {
  gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}