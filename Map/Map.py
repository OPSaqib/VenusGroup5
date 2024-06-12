import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
import numpy as np

def create_3d_grid(ax, pos_length, neg_length, width, height=10):
    for i in range(-neg_length, pos_length + 1):
        for j in range(width + 1):
            ax.plot([i, i], [j, j], [0, height], color='gray', linestyle='dotted')
        for j in range(height + 1):
            ax.plot([i, i], [0, width], [j, j], color='gray', linestyle='dotted')
    for i in range(width + 1):
        for j in range(height + 1):
            ax.plot([-neg_length, pos_length], [i, i], [j, j], color='gray', linestyle='dotted')

def draw_cube(ax, position, size, color, height=1):
    x, y = position
    z = 0
    vertices = [
        [(x, y, z), (x + size, y, z), (x + size, y + size, z), (x, y + size, z)],
        [(x, y, z + height), (x + size, y, z + height), (x + size, y + size, z + height), (x, y + size, z + height)],
        [(x, y, z), (x + size, y, z), (x + size, y, z + height), (x, y, z + height)],
        [(x, y + size, z), (x + size, y + size, z), (x + size, y + size, z + height), (x, y + size, z + height)],
        [(x, y, z), (x, y + size, z), (x, y + size, z + height), (x, y, z + height)],
        [(x + size, y, z), (x + size, y + size, z), (x + size, y + size, z + height), (x + size, y, z + height)],
    ]
    poly3d = Poly3DCollection(vertices, facecolors=color, linewidths=1, edgecolors='black', alpha=.5)
    ax.add_collection3d(poly3d)

def draw_mountain(ax, coordinates, height=10):
    vertices = []
    for (x, y) in coordinates:
        base = [(x, y, 0), (x + 1, y, 0), (x + 1, y + 1, 0), (x, y + 1, 0)]
        peak = [(x + 0.5, y + 0.5, height)]
        vertices.extend([
            base,
            [base[0], base[1], peak[0]],
            [base[1], base[2], peak[0]],
            [base[2], base[3], peak[0]],
            [base[3], base[0], peak[0]]
        ])
    poly3d = Poly3DCollection(vertices, facecolors='brown', linewidths=1, edgecolors='black', alpha=.7)
    ax.add_collection3d(poly3d)

def draw_cliff(ax, coordinates):
    for (x, y) in coordinates:
        draw_cube(ax, (x, y), 1, 'black', height=0.05)

def main():
    element_positions = {
        'stone': [],
        'mountain': [],
        'cliff': []
    }

    while True:
        element_type = input("Select the element you found (stone, mountain, cliff) or enter 'exit' to finish: ")
        if element_type.lower() == 'exit':
            break
        elif element_type.lower() == 'stone':
            while True:
                coordinate_input = input("Enter the coordinates of the stone (x,y) or 'done' to finish: ")
                if coordinate_input.lower() == 'done':
                    break
                try:
                    x, y = map(int, coordinate_input.split(','))
                    size_input = input("Enter the size of the stone (1, 2): ")
                    color = input("Enter the color of the stone ('r', 'g', 'b', 'k(black)', 'w'.): ")
                    size = int(size_input)
                    element_positions['stone'].append((x, y, size, color))
                except ValueError:
                    print("Enter valid coordinates in the format x,y and a valid size.")
                    continue

        elif element_type.lower() == 'mountain':
            while True:
                coordinate_input = input("Enter the coordinates of the mountain (x,y) or 'done' to finish: ")
                if coordinate_input.lower() == 'done':
                    break
                try:
                    x, y = map(int, coordinate_input.split(','))
                    element_positions['mountain'].append((x, y))
                except ValueError:
                    print("Enter valid coordinates in the format x,y.")
                    continue

        elif element_type.lower() == 'cliff':
            while True:
                coordinate_input = input("Enter the coordinates of the cliff (x,y) or 'done' to finish: ")
                if coordinate_input.lower() == 'done':
                    break
                try:
                    x, y = map(int, coordinate_input.split(','))
                    element_positions['cliff'].append((x, y))
                except ValueError:
                    print("Enter valid coordinates in the format x,y.")
                    continue

        else:
            print("Unknown element")

    while True:
        pos_length_input = input("Enter the positive length of the grid: ")
        neg_length_input = input("Enter the negative length of the grid: ")
        width_input = input("Enter the width of the grid: ")
        try:
            pos_length = int(pos_length_input)
            neg_length = int(neg_length_input)
            width = int(width_input)
            if pos_length <= 0 or neg_length <= 0 or width <= 0:
                print("Please enter positive integers for the lengths and width.")
                continue
            break
        except ValueError:
            print("Please enter valid integers for the lengths and width.")
    
    height = 10
    occupied_positions = set()
   
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')

    create_3d_grid(ax, pos_length, neg_length, width, height)

    for stone in element_positions['stone']:
        x, y, size, color = stone
        if -neg_length <= x <= pos_length and 0 <= y <= width:
            draw_cube(ax, (x, y), size, color)
            occupied_positions.add((x, y))
    
    draw_mountain(ax, element_positions['mountain'])
    draw_cliff(ax, element_positions['cliff'])

    ax.set_xlim([-neg_length, pos_length])
    ax.set_ylim([0, width])
    ax.set_zlim([0, height])

    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')

    ax.set_box_aspect([neg_length + pos_length, width, height])

    ax.set_xticks(range(-neg_length, pos_length + 1))
    ax.set_yticks(range(width + 1))
    ax.set_zticks(range(height + 1))
   
    plt.show()

if __name__ == "__main__":
    main()
