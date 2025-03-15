import cv2
import numpy as np
import os
import subprocess
import xml.etree.ElementTree as ET
import json
import shutil
import zipfile

def klei_to_mine_index(input_path):
    w = 136
    h = 136
    img = cv2.imread(input_path, -1)
    if img.shape[0] != 1024:
        print("wrong shape: ", img.shape)
        return
    klei_map = {}
    klei_atlas = {}
    for row in range(7):
        for col in range(7):
            local_img = img[row * h: (row+1) * h, col * w: (col + 1) * w]

            atlas = 0
            atlas = atlas | ((local_img[4, 4, 3] > 0) << 0)
            atlas = atlas | ((local_img[4, w - 5, 3] > 0) << 1)
            atlas = atlas | ((local_img[h - 5, w - 5, 3] > 0) << 2)
            atlas = atlas | ((local_img[h - 5, 4, 3] > 0) << 3)

            atlas = atlas | ((local_img[4, int(w / 2), 3] > 0) << 4)
            atlas = atlas | ((local_img[int(h / 2), w - 5, 3] > 0) << 5)
            atlas = atlas | ((local_img[h - 5, int(w / 2), 3] > 0) << 6)
            atlas = atlas | ((local_img[int(h / 2), 4, 3] > 0) << 7)

            atlas = atlas | ((local_img[int(h / 2), int(w / 2), 3] > 0) << 8)
            if (row != 6) or (col != 6):
                klei_map[row * 7 + col] = atlas
                klei_atlas[row * 7 + col] = local_img
    return klei_map
    
    atlas_map = {
        0b0000: 0,
        0b0001: 1,
        0b0010: 2,
        0b0100: 3,
        0b1000: 4,
        0b0011: {
            0b0001: {0b0: 5, 0b1: 6}
        },
        0b0101: 7,
        0b1001: {
            0b1000: {0b0: 8, 0b1000: 9}
        },
        0b0110: {
            0b0010: {0b0: 10, 0b0010: 11}
        },
        0b1010: 12,
        0b1100: {
            0b0100: {0b0: 13, 0b0100: 14}
        },
        0b0111: {
            0b0011: {
                0b00: 15,
                0b01: 16,
                0b10: 17,
                0b11: 18
            }
        },
        0b1011: {
            0b1001: {
                0b0000: 19,
                0b0001: 20,
                0b1000: 21,
                0b1001: 22
            }
        },
        0b1101: {
            0b1100: {
                0b0000: 23,
                0b0100: 24,
                0b1000: 25,
                0b1100: 26
            }
        },
        0b1110: {
            0b0110: {
                0b0000: 27,
                0b0010: 28,
                0b0100: 29,
                0b0110: 30
            }
        },
        0b1111: {
            0b11111: 47,
            0b1111: {
                0b0000: 31,
                0b0001: 32,
                0b0010: 33,
                0b0100: 34,
                0b1000: 35,
                0b0011: 36,
                0b0101: 37,
                0b1001: 38,
                0b0110: 39,
                0b1010: 40,
                0b1100: 41,
                0b0111: 42,
                0b1011: 43,
                0b1101: 44,
                0b1110: 45,
                0b1111: 46
            }
        }
    }

    index_map = {}
    for klei_atlas_index, atlas in klei_map.items():
        first_four = atlas & 0b1111
        level2 = atlas_map[first_four]
        if isinstance(level2, int):
            target = level2
        else:
            dict2 = level2
            atlas2 = atlas >> 4
            for key, value in dict2.items():
                if isinstance(value, int):
                    if atlas2 == key:
                        target = value
                        break
                else:
                    dict3 = value
                    target = dict3[atlas2 & key]
        t_row = int(target / 7)
        t_col = target % 7
        row = int(klei_atlas_index / 7)
        col = klei_atlas_index % 7
        index_map[klei_atlas_index] = target
    return index_map

def convert(input_path, out_path):
    if not os.path.exists(input_path + ".png") or not os.path.exists(input_path + ".xml"):
        return
    out_json = {}
    out_json['name'] = os.path.split(input_path)[-1]
    out_json['size'] = None
    out_json['tiles'] = []

    tree = ET.parse(input_path + ".xml")
    root = tree.getroot()
    img = cv2.imread(input_path + ".png", -1)
    # index_map = {0: 47, 1: 9, 2: 6, 3: 22, 4: 11, 5: 40, 6: 18, 7: 43, 8: 14, 9: 26, 10: 37, 11: 44, 12: 30, 13: 45, 14: 42, 15: 46, 16: 0, 17: 1, 18: 2, 19: 5, 20: 3, 21: 7, 22: 10, 23: 15, 24: 4, 25: 8, 26: 12, 27: 19, 28: 13, 29: 23, 30: 27, 31: 31, 32: 16, 33: 28, 34: 24, 35: 21, 36: 32, 37: 33, 38: 34, 39: 35, 40: 20, 41: 17, 42: 29, 43: 25, 44: 38, 45: 36, 46: 41, 47: 39}
    for element in root.findall("./Elements/Element"):
        idx = int(element.get("name"))
        u1 = float(element.get("u1"))
        u2 = float(element.get("u2"))
        v1 = float(element.get("v1"))
        v2 = float(element.get("v2"))
        if(out_json["size"] is None):
            out_json['size'] = [int((u2 - u1) * img.shape[1]), int((v2 - v1) * img.shape[0])]

        out_json['tiles'].append({
            # 'coord': [int((idx - 1) / 7), (idx - 1) % 7],
            'flag': int(klei_map[idx - 1]),
            'x': int(u1 * img.shape[1]),
            'y': int((1-v2) * img.shape[0]),
        })
    # out_atlas_img = np.zeros(img.shape, dtype=img.dtype)
    # for src_idx, tgt_idx in index_map.items():
    #     src_row = int(src_idx / 7)
    #     src_col = src_idx % 7
    #     tgt_row = int(tgt_idx / 7)
    #     tgt_col = tgt_idx % 7
    #     out_atlas_img[tgt_row * h:(tgt_row + 1) * h, tgt_col * w:(tgt_col + 1) * w] = img[src_row * h:(src_row + 1) * h, src_col * w:(src_col + 1) * w]
    # cv2.imwrite(out_path, out_atlas_img)
    shutil.copy(input_path + ".png", out_path + ".png")
    json.dump(out_json, open(out_path + ".json", 'w'))

def recursive_tex_convert(input_path):
    for root, folder, files in os.walk(input_path):
        if root == "E:/Project/data\\images\\colour_cubes":
            continue
        for file in files:
            base_name, ext = os.path.splitext(file)
            if ext != ".tex":
                continue
            png_path = os.path.join(root, base_name + ".png")
            if not os.path.exists(png_path):
                subprocess.run(["./TEXTool.exe", os.path.join(root, file), png_path])
                print(png_path)

def pack_new_zip(input_path, out_folder):
    for root, folders, files in os.walk(input_folder):
        if root != input_folder:
            continue
        for folder in folders:
            with zipfile.ZipFile(os.path.join(out_folder, folder + ".zip"), 'w', zipfile.ZIP_DEFLATED) as zipf:
                for file in os.listdir(os.path.join(root, folder)):
                    if file[-4:] == ".tex":
                        continue
                    zipf.write(os.path.join(root, folder, file), file)
                    if file[-4:] == ".png":
                        shutil.copyfile(os.path.join(root, folder, file), os.path.join(out_folder, "Brief", folder + ".png"))

if __name__ == "__main__":
    # klei_map = klei_to_mine_index("E:/Project/data/levels/tiles/blocky.png")

    # input_folder = "E:/Project/data"
    # recursive_tex_convert(input_folder)

    input_folder = R"E:\Project\data\anim"
    out_folder = R"E:\Project\Perforce\alive\Assets\Entities\Animations"
    pack_new_zip(input_folder, out_folder)

    # input_folder = "E:/Project/data/levels/tiles"
    # out_folder = "E:/Project/Perforce/alive/Assets/Map/TileMap/Terrain"
    # for root, folder, files in os.walk(input_folder):
    #     for file in files:
    #         base_name, ext = os.path.splitext(file)
    #         if ext != '.tex':
    #             continue
    #         input_path = os.path.join(root, base_name)
    #         print(input_path)
    #         convert(input_path, os.path.join(out_folder, base_name))
