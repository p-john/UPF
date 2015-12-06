import os, pickle

class StateObject:
    save_file = ""
    states = {}

    def __init__(self,save_file,states):
        self.states = states
        self.save_file = save_file
        print(save_file)
        if os.path.exists(save_file):
            self.states = pickle.load(open(self.save_file,"rb"))
            print("Old Save State found - Resuming")
        else:
            print("No Save State found")

    def update_state(self, tuple):
        self.states.update(tuple)
        pickle.dump(self.states,open(self.save_file,"wb"))
        print("State Saved")


