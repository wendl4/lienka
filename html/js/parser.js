const Commands = {
    up : 1,
    down : 2,
    right : 3,
    left : 4,
    increment : 5,
    decrement : 6,
    jumpIfZero : 7,
    jump : 8,
    assign : 9
}

class Parser {
    constructor(code) {
        this.code = code
        this.tree = null
        this.position = -1
    }

    Interpret() {
        let val = this.SyntaxAnalyse(this.code)
        if (val) {
            this.position = -1
            let tree = this.makeTree(this.code)
            return this.InterpretTree(tree)
        }
        else console.log(val)
        return false
    }

    SyntaxAnalyse(code) {
        let bracketsratio = 0
        while (this.position < code.length - 1) {
            this.position++
            let command = code[this.position]
            switch(command) {
                case "count2":
                case "count3":
                    if (code[this.position+1] != "begin") {
                        return "ERROR: Expecting left bracket"
                    }
                    break
                case "up":
                case "down":
                case "left":
                case "right":
                    if (code[this.position+1] == "begin") {
                        return "ERROR: Unexpected left bracket"
                    }
                    break
                case "begin":
                    bracketsratio++
                    if (code[this.position+1] == "begin" || this.position == 0) {
                        return "ERROR: Unexpected left bracket"
                    }
                    break
                case "end":
                    bracketsratio--
                    if (code[this.position+1] == "begin") {
                        return "ERROR: Unexpected left bracket"
                    }
                    if (this.position == 0 || bracketsratio < 0) {
                        return "ERROR: Unexpected right bracket"
                    }
                    break
                default: 
                    return "ERROR: Undefined command " + command + " " + this.position
                    break
            }
        }
        return (bracketsratio == 0) ? true : "ERROR: Brackets number not equal"
    }

    makeTree(code) {
        let root = new CommandNode()
        while (this.position < code.length) {
            this.position++
            let command = code[this.position]
            switch(command) {
                case "count2":
                    root.addCommand(new LoopNode(2,this.makeTree(code,this.position)))
                    break
                case "count3":
                    root.addCommand(new LoopNode(3,this.makeTree(code,this.position)))
                    break
                case "up":
                    root.addCommand(new Leaf("up"))
                    break
                case "down":
                    root.addCommand(new Leaf("down"))
                    break
                case "left":
                    root.addCommand(new Leaf("left"))
                    break
                case "right":
                    root.addCommand(new Leaf("right"))
                    break
                case "begin":
                    break
                case "end":
                    return root
                default: 
                    return root;
            }
        }
        return root
    }

    InterpretTree(tree,curReg = 0) {
        let currentRegister = curReg
        let output = ""
        if (tree instanceof CommandNode) {
            tree.commands.forEach(function (command) {
                output+=this.InterpretTree(command)
            }.bind(this))
        }
        else if (tree instanceof LoopNode) {
            output+= Commands.Assign  // assign instruction
            output+= tree.accumulator // value to be assigned
            output+= currentRegister // register address

            output+= Commands.jumpIfZero // 
            output+= currentRegister // address of register
            output+= 0 // todo jump index
            currentRegister++

            output+= this.InterpretTree(tree.body,currentRegister)
        }
        else if (tree instanceof Leaf) {
            output += Commands[tree.command]
        }

        return output
    }
}


class LoopNode {
    constructor(accumulator, body = null) {
        this.accumulator = accumulator
        this.body = body
    }
}

class CommandNode {
    constructor() {
        this.commands = []
    }

    addCommand(command) {
        this.commands.push(command)
    }
}

class Leaf {
    constructor(command) {
        this.command = command
    }
}
