import argparse
import json
import copy

camera = {
    "fov": 30,
    "position": {
      "x": 0,
      "y": 0,
      "z": 0
    },
    "rotation": {
      "x": 0,
      "y": 0,
      "z": 0
    }
}

pointlight = {
    "position": {
      "x": 0,
      "y": 100,
      "z": 0
    },
    "emission": {
      "x": 200,
      "y": 200,
      "z": 200
    }
}

plane = {
    "kind": "plane",
    "position": {
      "x": 0,
      "y": 0,
      "z": 0
    },
    "rotation": {
      "x": 0,
      "y": 0,
      "z": 0
    },
    "reflection": 0,
    "shininess": 15,
    "color": {
      "x": 1,
      "y": 1,
      "z": 1
    },
    "params": {
      "normal": {
        "x": 0,
        "y": 1,
        "z": 0
      },
      "displacement": -4
    }
}

box = {
    "kind": "box",
    "position": {
      "x": -4,
      "y": -1.5,
      "z": 15
    },
    "rotation": {
      "x": -1.5,
      "y": -1.5,
      "z": 12
    },
    "reflection": 0,
    "shininess": 15,
    "color": {
      "x": 0,
      "y": 0.1,
      "z": 0.6
    },
    "params": {
      "extents": {
        "x": 0.25,
        "y": 0.5,
        "z": 1
      }
    }
}

sphere = {
    "kind": "sphere",
    "position": {
      "x": 0,
      "y": 0,
      "z": 20
    },
    "rotation": {
      "x": 0,
      "y": 0,
      "z": 0
    },
    "reflection": 0.5,
    "shininess": 15,
    "color": {
      "x": 0.3,
      "y": 0,
      "z": 0
    },
    "params": {
      "radius": 1.5
    }
}

cone = {
    "kind": "cone",
    "position": {
      "x": 1.5,
      "y": -1.5,
      "z": 12
    },
    "rotation": {
      "x": 0,
      "y": 0,
      "z": 0
    },
    "reflection": 0,
    "shininess": 15,
    "color": {
      "x": 0,
      "y": 0.1,
      "z": 0.6
    },
    "params": [
      1,
      0.5,
      1
    ]
}

torus = {
    "kind": "torus",
    "position": {
      "x": 4,
      "y": -1.5,
      "z": 15
    },
    "rotation": {
      "x": 0,
      "y": 0,
      "z": 0
    },
    "reflection": 0,
    "shininess": 15,
    "color": {
      "x": 0.5,
      "y": 0.5,
      "z": 0
    },
    "params": {
      "r1": 1,
      "r2": 0.5
    }
}

octa = {
    "kind": "octahedron",
    "position": {
      "x": -1.5,
      "y": -1.5,
      "z": 12
    },
    "rotation": {
      "x": 0,
      "y": 0,
      "z": 0
    },
    "reflection": 0,
    "shininess": 15,
    "color": {
      "x": 0,
      "y": 0.6,
      "z": 0.1
    },
    "params": {
      "s": 1
    }
}

def main(args):
    # parse arguments
    if args['scenenr']:
        scene_number = str(args['scenenr'])
    else:
        scene_number = "10"
    filename = f"scene{scene_number}.json"

    if args['shapes']:
        nr_plane = int(args['shapes'][0])
        nr_box = int(args['shapes'][1])
        nr_sphere = int(args['shapes'][2])
        nr_cone = int(args['shapes'][3])
        nr_torus = int(args['shapes'][4])
        nr_octa = int(args['shapes'][5])


    objects = []
    displacement = -4
    for i in range(nr_plane):
        this_plane = copy.deepcopy(plane)
        this_plane['params']['displacement'] = displacement
        objects.append(this_plane)
        displacement -= 1

    pos_x = 3
    pos_y = 2
    pos_z = 10
    rot_z = 1.5
    for i in range(nr_box):
        this_box = copy.deepcopy(box)
        this_box['position']['x'] = pos_x
        this_box['position']['y'] = pos_y
        this_box['position']['z'] = pos_z
        this_box['rotation']['z'] = rot_z
        objects.append(this_box)
        pos_z += 3
        rot_z = rot_z + rot_z
        if rot_z >= 360:
            rot_z = 2.5

    pos_x = -3
    pos_y = 2
    pos_z = 10
    for i in range(nr_sphere):
        this_sphere = copy.deepcopy(sphere)
        this_sphere['position']['x'] = pos_x
        this_sphere['position']['y'] = pos_y
        this_sphere['position']['z'] = pos_z
        objects.append(this_sphere)
        pos_z += 3

    pos_x = 3
    pos_y = -2
    pos_z = 10
    rot_z = 1.5
    for i in range(nr_cone):
        this_cone = copy.deepcopy(cone)
        this_cone['position']['x'] = pos_x
        this_cone['position']['y'] = pos_y
        this_cone['position']['z'] = pos_z
        this_cone['rotation']['z'] = rot_z
        objects.append(this_cone)
        pos_z += 3
        rot_z = rot_z + rot_z
        if rot_z >= 360:
            rot_z = 2.5

    pos_x = -3
    pos_y = -2
    pos_z = 10
    rot_z = 1.5
    for i in range(nr_torus):
        this_torus = copy.deepcopy(torus)
        this_torus['position']['x'] = pos_x
        this_torus['position']['y'] = pos_y
        this_torus['position']['z'] = pos_z
        this_torus['rotation']['z'] = rot_z
        objects.append(this_torus)
        pos_z += 3
        rot_z = rot_z + rot_z
        if rot_z >= 360:
            rot_z = 2.5

    pos_x = 0
    pos_y = -3
    pos_z = 10
    rot_z = 1.5
    for i in range(nr_octa):
        this_octa = copy.deepcopy(octa)
        this_octa['position']['x'] = pos_x
        this_octa['position']['y'] = pos_y
        this_octa['position']['z'] = pos_z
        this_octa['rotation']['z'] = rot_z
        objects.append(this_octa)
        pos_z += 3
        rot_z = rot_z + rot_z
        if rot_z >= 360:
            rot_z = 2.5

    scene_dict = {
        "camera": camera,
        "pointlight": pointlight,
        "objects": objects
    }
    with open(filename, "w+") as scenefile:
        print(f"Writing to {filename}")
        json.dump(scene_dict, scenefile)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-n', '--scenenr', type=int, help='The number of the scene file')
    parser.add_argument('-s', '--shapes', type=int, nargs=6, help='Amount of shapes to be built as list in order [plane, box, sphere, cone, torus, octahedron]', required=True)
    #parser.add_argument('-r', '--random', action='store_true', default=False, help='Get random positions')
    args = vars(parser.parse_args())
    main(args)