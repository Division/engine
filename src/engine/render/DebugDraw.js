import app from '../Application';
import math from 'math';
import Mesh from 'engine/render/Mesh';
const { vec3 } = math;
const { floor } = Math;

const MAX_VERTICES = 1000;
const VERTEX_COMPONENT_COUNT = 3;
const COLOR_COMPONENT_COUNT = 4;
const COMPONENT_COUNT = VERTEX_COMPONENT_COUNT + COLOR_COMPONENT_COUNT;

export default class DebugDraw {

  constructor (opts) {
    let gl = this.gl = app.gl;
    this.renderer = app.instance.renderer;

    this.shader = opts.shader;
    this.renderOpts = { renderMode: gl.LINES };

    this.linesMesh = new Mesh({
      componentCount: 2
    });

    this.pointsMesh = new Mesh({
      componentCount: 1
    });

    this.lineCount = 0;
    this.pointCount = 0;
    this.vertexCount = 0;

    this.vertexData = new Float32Array(MAX_VERTICES * COMPONENT_COUNT);

    this.vbo = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, this.vbo);
    gl.bufferData(gl.ARRAY_BUFFER, this.vertexData, gl.DYNAMIC_DRAW);

    this.linesMesh.setVBO(this.vbo);
    this.linesMesh.strideBytes = COMPONENT_COUNT * 4;
    this.linesMesh.colorOffsetBytes = VERTEX_COMPONENT_COUNT * 4;
    this.linesMesh.hasColors = true;
    this.linesMesh.hasVertices = true;

    this.pointsMesh.setVBO(this.vbo);
    this.pointsMesh.strideBytes = COMPONENT_COUNT * 4;
    this.pointsMesh.colorOffsetBytes = VERTEX_COMPONENT_COUNT * 4;
    this.pointsMesh.hasColors = true;
    this.pointsMesh.hasVertices = true;

    this.vertexCount = 0;
  }

  addPoint (p, color) {
    this.addPointXYZ(p[0], p[1], p[2], color);
  }

  addPointXYZ (x, y, z, color) {
    let count = this.vertexCount;

    this.vertexData[count * COMPONENT_COUNT] = x;
    this.vertexData[count * COMPONENT_COUNT + 1] = y;
    this.vertexData[count * COMPONENT_COUNT + 2] = z;
    this.vertexData[count * COMPONENT_COUNT + 3] = color[0];
    this.vertexData[count * COMPONENT_COUNT + 4] = color[1];
    this.vertexData[count * COMPONENT_COUNT + 5] = color[2];
    this.vertexData[count * COMPONENT_COUNT + 6] = 1;

    this.vertexCount += 1;
    this.pointCount += 1;
  }

  addLine (p1, p2, color) {
    this.addLineXYZ(p1[0], p1[1], p1[2], p2[0], p2[1], p2[2], color);
  }

  addLineXYZ(x1, y1, z1, x2, y2, z2, color) {
    let count = this.vertexCount;

    for (let i = 0; i < 2; i++) {
      this.vertexData[count * COMPONENT_COUNT] = arguments[i * VERTEX_COMPONENT_COUNT];
      this.vertexData[count * COMPONENT_COUNT + 1] = arguments[i * VERTEX_COMPONENT_COUNT + 1];
      this.vertexData[count * COMPONENT_COUNT + 2] = arguments[i * VERTEX_COMPONENT_COUNT + 2];
      this.vertexData[count * COMPONENT_COUNT + 3] = color[0];
      this.vertexData[count * COMPONENT_COUNT + 4] = color[1];
      this.vertexData[count * COMPONENT_COUNT + 5] = color[2];
      this.vertexData[count * COMPONENT_COUNT + 6] = 1;
      count++;
    }

    this.lineCount += 1;
    this.vertexCount = count;
  }

  clear () {
    this.vertexCount = 0;
    this.lineCount = 0;
    this.pointCount = 0;
  }

  updateBuffer () {
    let gl = this.gl;
    gl.bindBuffer(gl.ARRAY_BUFFER, this.vbo);
    gl.bufferSubData(gl.ARRAY_BUFFER, 0, this.vertexData, 0, this.vertexCount * COMPONENT_COUNT * 4);

    this.linesMesh.faceCount = this.lineCount;
    this.pointsMesh.faceCount = this.pointCount;
  }

  render () {
    if (this.vertexCount === 0) {
      return;
    }

    let gl = this.gl;
    gl.disable(gl.DEPTH_TEST);
    this.updateBuffer();

    if (this.lineCount) {
      this.renderOpts.renderMode = gl.LINES;
      this.renderer.renderMesh(this.linesMesh, this.shader, null, this.renderOpts);
    }

    if (this.pointCount) {
      this.renderOpts.renderMode = gl.POINTS;
      this.renderer.renderMesh(this.pointsMesh, this.shader, null, this.renderOpts);
    }

    this.clear();
  }
}

DebugDraw.GREEN = [0.1, 9.8, 0.1];
DebugDraw.RED = [0.8, 0.1, 0.1];
DebugDraw.BLUE = [142/255, 208/255, 255/255];
DebugDraw.ORANGE = [1, 161/255, 40/255];
DebugDraw.LIME = [224/255, 234/255, 180/255];